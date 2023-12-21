document.addEventListener("DOMContentLoaded", function () {
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
            addMarker(messages);
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
            messages.forEach(messageData => {
                const listItem = document.createElement('li');
                const timestamp = new Date(parseInt(messageData.timestamp) * 1000).toLocaleString(); // Parse to integer and Format timestamp
                listItem.textContent = `${timestamp}: Accident detected at location : ${messageData.latitude}, ${messageData.longitude}`;
                messageList.appendChild(listItem);
            });
        }
    } else {
        console.error('Invalid messages format:', messages);
    }
}

let marker, circle, zoom;
function addMarker(messages) {
    // const latitude = parseFloat(messageData.latitude);
    // const longitude = parseFloat(messageData.longitude);
    const coordinates = messages
        .filter(messageData => !isNaN(parseFloat(messageData.latitude)) && !isNaN(parseFloat(messageData.longitude)))
        .map(messageData => [parseFloat(messageData.latitude), parseFloat(messageData.longitude)]);

    // map.eachLayer(layer => {
    //     if (layer instanceof L.Marker) {
    //         map.removeLayer(layer);
    //     }
    // });

    // Add markers to the map based on JSON data

    coordinates.forEach(coordinate => {
        // Convert coordinate to a string for use as a dictionary key    
        if (!isMarkerAlreadyAdded(coordinate)) {
            // Add a new marker and circle
            const marker = L.marker(coordinate).addTo(map);
            const circle = L.circle(coordinate, {
                color: 'red',
                fillColor: '#f03',
                fillOpacity: 0.1,
                radius: 100
            }).addTo(map);
    
            marker.on('click', function () {
                // Open the popup    
                // Zoom the map to the marker's location
                marker.bindPopup(`<b>Accident detected!</b><br>Coordinates: ${coordinate}`).openPopup();
                map.fitBounds(circle.getBounds()); // Adjust the zoom level as needed
            });
    
            // Mark the coordinate as having an existing marker
        }
    });
    // if (!zoom){
    //     zoom = map.setView(coordinate, 14);
    // }
}

function isMarkerAlreadyAdded(coordinate) {
    let markerAlreadyAdded = false;

    // Iterate through all layers on the map
    map.eachLayer(layer => {
        // Check if the layer is a marker and has the same coordinates
        if (layer instanceof L.Marker && layer.getLatLng().equals(coordinate)) {
            markerAlreadyAdded = true;
        }
    });

    return markerAlreadyAdded;
}