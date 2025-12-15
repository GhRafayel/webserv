const BASE_URL = "http://localhost:8080";

function sendGet() {
    fetch(`${BASE_URL}/get`)
        .then(res => res.text())
        .then(data => {
            document.getElementById("get-result").textContent = data;
        })
        .catch(err => console.error(err));
}

function sendPost() {
    const value = document.getElementById("post-input").value;

    fetch(`${BASE_URL}/post`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({ message: value })
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
