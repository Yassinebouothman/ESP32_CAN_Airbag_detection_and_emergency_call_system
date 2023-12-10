document.addEventListener("DOMContentLoaded", function() {
    // Fetch messages from the server
    fetchMessages();

    // Fetch messages every 5 seconds (adjust as needed)
    setInterval(fetchMessages, 5000);
});

function fetchMessages() {
    // Make an AJAX request to get messages from the server
    fetch('messages.json')
        .then(response => response.json())
        .then(messages => {
            // Update the HTML to display messages
            displayMessages(messages);
        })
        .catch(error => {
            console.error('Error fetching messages:', error);
        });
}

function displayMessages(messages) {
    const messageList = document.getElementById('messageList');
    messageList.innerHTML = ''; // Clear previous messages

    if (Array.isArray(messages)) {
        if (messages.length === 0) {
            const listItem = document.createElement('li');
            listItem.textContent = 'No messages available.';
            messageList.appendChild(listItem);
        } else {
            messages.forEach(message => {
                const listItem = document.createElement('li');
                listItem.textContent = message;
                messageList.appendChild(listItem);
            });
        }
    } else {
        console.error('Invalid messages format:', messages);
    }
}
