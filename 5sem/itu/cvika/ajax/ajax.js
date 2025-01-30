const READY_STATE_FINISHED = 4;
const STATUS_OK = 200;
const API = "https://pckiss.fit.vutbr.cz/itu/api.php";

// message.id -> message
var messages = new Map();

function createXmlHttpRequestObject() {
    var request;

    try {
        request = new XMLHttpRequest(); // should work on all browsers except IE6 or older
    }
    catch (e) {
        try {
            request = new ActiveXObject("Microsoft.XMLHttp"); // browser is IE6 or older
        }
        catch (e) {
            // ignore error
        }
    }

    if (!request) {
        alert("Error creating the XMLHttpRequest object.");
    }
    else {
        return request;
    }
}

function uploadData() {
    var message_box = document.getElementById("MessageString");
    var login_box = document.getElementById("LoginString");
    var message = message_box.value;
    var login = login_box.value;
    message_box.value = "";
    console.log(`sending message '${message}'`);

    try {
        var request = createXmlHttpRequestObject();
        request.open("POST", API, true);
        request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        request.onreadystatechange = function() {
            if (request.readyState == READY_STATE_FINISHED
                && request.status == STATUS_OK) {
                downloadData();
            }
        }

        request.send(`user=${encodeURIComponent(login)}&data=${encodeURIComponent(message)}`);
    }
    catch (e) {
        alert(e.toString());
    }

    return false; // to avoid default form submit behavior 
}

function add_new_messages(json) {
    return JSON
        .parse(json)
        .map((message) => {
            if (!messages.has(message.id)) {
                document.getElementById("status").innerHTML += `${message.id} `;
                messages.set(message.id, message)
            }
        });
}

function render_messages() {
    return Array.from(messages.values())
        .map((message) => {
            return `<span style='color: grey'>${message.login}</span>: ${message.cnt}`
        }).join("<br>");
}

function downloadData() {
    console.log("getting messages");
    var request = createXmlHttpRequestObject();

    request.open("GET", API, true);

    request.onreadystatechange = function() {
        if (request.readyState == READY_STATE_FINISHED
            && request.status == STATUS_OK) {
            add_new_messages(request.responseText);
            var chat_area = document.getElementById("chatArea");
            chat_area.innerHTML = render_messages();
        }
    }

    request.send(null);
}

downloadData()
setInterval(downloadData, 1000)

//// put your code here

