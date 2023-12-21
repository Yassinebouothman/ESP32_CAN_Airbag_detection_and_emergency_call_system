<?php
// Check if the request is a POST request
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get the JSON data from the request body
    $postData = file_get_contents('php://input');

    // Decode the JSON data
    $messageData = json_decode($postData, true);

    // Validate and process the message data as needed
    if ($messageData !== null) {

        $latitude = $messageData['latitude'];
        $longitude = $messageData['longitude'];
        $timestamp = time();
        // Simulate storing the message in a database or file
    
        // Replace this with your actual logic to handle the incoming message
        $messagesFile = 'messages.json';
        $existingMessages = json_decode(file_get_contents($messagesFile), true) ?? [];
        $existingMessages[] = ['latitude' => $latitude,'longitude' => $longitude,'timestamp' => $timestamp];

        // Save the updated messages to the file
        file_put_contents($messagesFile, json_encode($existingMessages));

        // Send a response (optional)
        header('Content-Type: application/json');
        echo json_encode(['status' => 'success']);
    } else {
        // Invalid JSON data
        header('Content-Type: application/json');
        echo json_encode(['status' => 'error', 'message' => 'Invalid JSON data']);
    }
} else {
    // Not a POST request
    header('Content-Type: application/json');
    echo json_encode(['status' => 'error', 'message' => 'Invalid request method']);
}
?>
