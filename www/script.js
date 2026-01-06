const BASE_URL = "http://localhost:8080";

function sendGet() {
     fetch(`${BASE_URL}/upload/data.json`)
        .then(res => res.ok ? res.text() : document.getElementById("get-result").textContent = "Get failed")
        .then(data => {
            document.getElementById("get-result").textContent = data;
        })
        .catch(err => console.error(err));
}

function sendPost() {
    const value = document.getElementById("post-input").value;

    fetch(`${BASE_URL}/upload/data.json`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify(
            {
                message:        value,
            })
    })
    .then(res => res.text())
    .then(data => {
        document.getElementById("post-result").textContent = data;
    })
    .catch(err => console.error(err));
}

function sendDelete() {
  fetch(`${BASE_URL}/upload/data.json`, {
    method: 'DELETE',
    headers:
    { 
        'Content-Type': 'application/json'
    } })
  .then(res => {
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    return res.text();
  })
  .then(data => {
    document.getElementById('delete-result').textContent = data;
  })
  .catch(err => {
    console.error(err);
    document.getElementById('delete-result').textContent = 'Delete failed';
  });
}