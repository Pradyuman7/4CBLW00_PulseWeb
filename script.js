const questionContainer = document.getElementById("question-container");
const submitBtn = document.getElementById("submit");
const backBtn = document.getElementById("backBtn");
const questionnaire = document.getElementById("questionnaire");
const dashboard = document.getElementById("dashboard");
const ctx = document.getElementById("trendChart").getContext("2d");
const categories = {
    mentalHealth: [
        "I feel emotionally balanced most of the time.",
        "I find it easy to focus on tasks at work.",
        "I feel confident in my ability to handle work challenges.",
        "I often experience a sense of fulfillment in my work.",
        "I am able to recognize and manage my emotions effectively.",
        "I feel mentally present and engaged throughout the workday.",
        "I feel hopeful about my future both personally and professionally.",
        "I am satisfied with my overall mental well-being.",
        "I can maintain a positive outlook even during difficult times.",
        "I feel in control of my thoughts and emotions during the workday."
    ],
    stress: [
        "I have felt anxious or overwhelmed recently.",
        "I feel stressed or under pressure frequently.",
        "I feel I have a healthy work-life balance.",
        "I can take breaks or time off when needed without guilt.",
        "I have enough support to manage my workload.",
        "I feel exhausted or emotionally drained by my work.",
        "I worry about meeting deadlines or expectations at work.",
        "I feel tense or irritable during or after work hours.",
        "I find it hard to disconnect from work during personal time.",
        "I am sleeping well and feel rested during the day."
    ],
    cohesion: [
        "I feel comfortable being myself at work.",
        "I feel valued by my team.",
        "I have someone at work I can talk to when I'm struggling.",
        "I feel included in team discussions and decisions.",
        "My manager or team lead shows genuine care for team well-being.",
        "I trust the people I work with.",
        "I feel a sense of belonging within my team or department.",
        "Team members support each other through challenges.",
        "Open communication is encouraged and practiced in my team.",
        "I feel motivated when working with my team."
    ]
};

let currentQuestionIndex = 0;
let selectedQuestions = [];
let answers = {};

function loadQuestions() {
    function getRandomItems(arr, count) {
        const shuffled = [...arr].sort(() => 0.5 - Math.random());
        return shuffled.slice(0, count);
    }

    const selectedMental = getRandomItems(categories.mentalHealth, 2);
    const selectedStress = getRandomItems(categories.stress, 2);
    const selectedCohesion = getRandomItems(categories.cohesion, 2);

    selectedQuestions = [...selectedMental, ...selectedStress, ...selectedCohesion]
        .sort(() => Math.random() - 0.5); // shuffle for mixed order

    currentQuestionIndex = 0;
    answers = {};
    submitBtn.classList.add("hidden");
    displayQuestion();
}


function displayQuestion() {
    const qText = selectedQuestions[currentQuestionIndex];
    questionContainer.innerHTML = `
    <div class="question" data-question-index="${currentQuestionIndex}">
      <p>${qText}</p>
      <button class="option" data-value="agree">Agree</button>
      <button class="option" data-value="disagree">Disagree</button>
    </div>
  `;

    document.querySelectorAll(".option").forEach(button => {
        button.addEventListener("click", () => {
            document.querySelectorAll(".option").forEach(b => b.classList.remove("selected"));
            button.classList.add("selected");
            answers[currentQuestionIndex] = button.getAttribute("data-value");

            setTimeout(() => {
                currentQuestionIndex++;
                if (currentQuestionIndex < selectedQuestions.length) {
                    displayQuestion();
                } else {
                    submitBtn.classList.remove("hidden");
                    questionContainer.innerHTML = `<p>You're done! Click submit to see your results.</p>`;
                }
            }, 300);
        });
    });
}

submitBtn.addEventListener("click", () => {
    if (Object.keys(answers).length < selectedQuestions.length) {
        alert("Please answer all questions.");
        return;
    }

    questionnaire.classList.add("hidden");
    dashboard.classList.remove("hidden");
    renderChart();
});

backBtn.addEventListener("click", () => {
    dashboard.classList.add("hidden");
    questionnaire.classList.remove("hidden");
    loadQuestions();
});

function renderChart() {
    const baseline = {
        mentalHealth: 0.55,
        stress: 0.35,
        cohesion: 0.70
    };

    let scores = {
        mentalHealth: { agree: 0, total: 0 },
        stress: { agree: 0, total: 0 },
        cohesion: { agree: 0, total: 0 }
    };

    selectedQuestions.forEach((qText, i) => {
        const response = answers[i];
        for (let category in categories) {
            if (categories[category].includes(qText)) {
                scores[category].total++;
                if (response === "agree") scores[category].agree++;
            }
        }
    });

    const userScores = Object.keys(scores).map(category =>
        scores[category].total > 0 ? scores[category].agree / scores[category].total : null
    );

    const baselineScores = Object.keys(baseline).map(k => baseline[k]);

    // Destroy previous chart if exists
    if (window.trendChart instanceof Chart) {
        window.trendChart.destroy();
    }

    window.trendChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: ["Mental Health", "Stress", "Social Cohesion"],
            datasets: [
                {
                    label: "Baseline",
                    data: baselineScores,
                    borderColor: "#d1d5db",
                    fill: false,
                    borderDash: [5, 5]
                },
                {
                    label: "Your Trend",
                    data: userScores,
                    borderColor: "#3b82f6",
                    fill: false,
                    tension: 0.3
                }
            ]
        },
        options: {
            scales: {
                y: {
                    min: 0,
                    max: 1,
                    ticks: {
                        callback: val => `${val * 100}%`
                    }
                }
            },
            plugins: {
                tooltip: {
                    callbacks: {
                        label: ctx => `${ctx.dataset.label}: ${(ctx.parsed.y * 100).toFixed(1)}%`
                    }
                }
            }
        }
    });
}

// Initialize on load
loadQuestions();
