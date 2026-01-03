const BASE_URL = "http://localhost:8080";

function sendGet() {
     fetch(`${BASE_URL}/get/get.json`)
        .then(res => res.text())
        .then(data => {
            document.getElementById("get-result").textContent = data;
        })
        .catch(err => console.error(err));
}

function sendPost() {
    const value = document.getElementById("post-input").value;

    fetch(`${BASE_URL}/upload`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify(
            {
                message:        value,
                "Name" :        "Alex",
                "Last name" :   "valod",
                "pass" :        "1234"
            })
    })
    .then(res => res.text())
    .then(data => {
        document.getElementById("post-result").textContent = data;
    })
    .catch(err => console.error(err));
}

function sendDelete() {
    fetch(`${BASE_URL}/delete`, {
        method: "DELETE"
    })
    .then(res => res.text())
    .then(data => {
        document.getElementById("delete-result").textContent = data;
    })
    .catch(err => console.error(err));
}
