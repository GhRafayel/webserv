document.addEventListener('DOMContentLoaded', function() {
    const button = document.getElementById('clickButton');
    const message = document.getElementById('message');
    
    button.addEventListener('click', function() {
        message.textContent = 'Button was clicked!';
        
        // Change button color temporarily
        button.style.backgroundColor = '#28a745';
        setTimeout(() => {
            button.style.backgroundColor = '';
        }, 300);
    });
});