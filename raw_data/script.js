var GLOBAL_host = "http://192.168.0.50";
function updateInnerHtmlForClassNames(className, innerHTMLValue) {
    var elements = document.getElementsByClassName(className);
    for (var i = 0; i < elements.length; i++) {
        elements[i].innerHTML = innerHTMLValue;
        //console.log(elements[i])
    }
}
function updateValueForId(Id, value) {
    var element = document.getElementById(Id);
    element.value = value;
}
function updateNodeValueForClassNames(className, nodeValue) {
    var elements = document.getElementsByClassName(className);
    for (var i = 0; i < elements.length; i++) {
        elements[i].nodeValue = nodeValue;
        //console.log(elements[i])
    }
}
function getWifiSvgForSignal(signalStrength)
{
    var signalColor = "#0cc90cb2";
    var wave3Color = "#a7a7a7";
    var wave2Color = wave3Color;
    var wave1Color = wave3Color;
    var dotColor = wave3Color;
    switch(getStrengthLabel(signalStrength))
    {
        case "High":
            wave3Color = signalColor;
            wave2Color = signalColor;
            wave1Color = signalColor;
            dotColor = signalColor;
            break;
        case "Mid":
            wave2Color = signalColor;
            wave1Color = signalColor;
            dotColor = signalColor;
            break;
        case "Low":
            wave1Color = signalColor;
            dotColor = signalColor;
            break;
        case "Weak":
            dotColor = signalColor;
            break;        
    }
    var wifiSvg = `<svg fill="#000000" height="30px" width="30px" version="1.1" id="Capa_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" viewBox="0 0 365.892 365.892" xml:space="preserve">
    <g id="SVGRepo_bgCarrier" stroke-width="0"></g>
    <g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"></g>
    <g id="SVGRepo_iconCarrier"> <g> 
        <circle fill="${dotColor}" cx="182.945" cy="286.681" r="41.494"></circle> 
        <path fill="${wave1Color}" d="M182.946,176.029c-35.658,0-69.337,17.345-90.09,46.398c-5.921,8.288-4.001,19.806,4.286,25.726 c3.249,2.321,6.994,3.438,10.704,3.438c5.754,0,11.423-2.686,15.021-7.724c13.846-19.383,36.305-30.954,60.078-30.954 c23.775,0,46.233,11.571,60.077,30.953c5.919,8.286,17.437,10.209,25.726,4.288c8.288-5.92,10.208-17.438,4.288-25.726 C252.285,193.373,218.606,176.029,182.946,176.029z"></path> 
        <path fill="${wave2Color}" d="M182.946,106.873c-50.938,0-99.694,21.749-133.77,59.67c-6.807,7.576-6.185,19.236,1.392,26.044 c3.523,3.166,7.929,4.725,12.32,4.725c5.051-0.001,10.082-2.063,13.723-6.116c27.091-30.148,65.849-47.439,106.336-47.439 s79.246,17.291,106.338,47.438c6.808,7.576,18.468,8.198,26.043,1.391c7.576-6.808,8.198-18.468,1.391-26.043 C282.641,128.621,233.883,106.873,182.946,106.873z"></path> 
        <path fill="${wave3Color}" d="M360.611,112.293c-47.209-48.092-110.305-74.577-177.665-74.577c-67.357,0-130.453,26.485-177.664,74.579 c-7.135,7.269-7.027,18.944,0.241,26.079c3.59,3.524,8.255,5.282,12.918,5.281c4.776,0,9.551-1.845,13.161-5.522 c40.22-40.971,93.968-63.534,151.344-63.534c57.379,0,111.127,22.563,151.343,63.532c7.136,7.269,18.812,7.376,26.08,0.242 C367.637,131.238,367.745,119.562,360.611,112.293z"></path> 
    </g> </g>
    </svg> `;
    return wifiSvg;
}
function connectToWifi(newSSID)
{
    var mySSID = document.getElementById("ssid");
    var newPassword = document.getElementById("password");
    mySSID.value = newSSID;
    newPassword.value = "";
    newPassword.focus();
}
function getStrengthLabel(rssi) {
    if (rssi >= -60) return 'High';
    else if (rssi >= -70) return 'Mid';
    else if (rssi >= -80) return 'Low';
    else return 'Weak';
}
function getEncryptionTypeIcon(encryptionType) {
    switch (encryptionType) {
        case 1: // Unencrypted
            return ``;
        default:
            return "Unidentified"; // Gray color for unknown encryption or other cases
    }
}
function getEncryptionType(encryptionType) {
    switch (encryptionType) {
        case 1: // Unencrypted
            return "Unencrypted"; // Red color for weak encryption
        case 2: // WEP
            return "WEP"; // Orange color for low encryption
        case 3: // WPA
            return "WPA"; // Yellow color for mid encryption
        case 4: // WPA2
            return "WPA2"; // Green color for high encryption
        case 8: // WPA2
            return "WPA/WPA2";
        default:
            return "Unidentified"; // Gray color for unknown encryption or other cases
    }
}
document.addEventListener('DOMContentLoaded', function() {
    document.getElementById('updateWifiCredBtn').addEventListener('click', function() {
        // Data to be sent
        document.getElementById('updateWifiCredBtn').disabled = true;
        document.getElementById('updateWifiCredBtnSpinner').style.display = 'block';
        
        // AJAX request
        var xhr = new XMLHttpRequest();
        xhr.open('POST', GLOBAL_host + '/api/updateWifiCred', true);
        //xhr.setRequestHeader('Content-Type', 'application/json');
        var formData = new FormData();
        formData.append('ssid', document.getElementById('ssid').value);
        formData.append('password', document.getElementById('password').value);
        xhr.onload = function() {
            if (xhr.status === 200) {
                console.log('Data sent successfully');
                alert('Updated Successfully - Status ' + JSON.parse(xhr.responseText).status);
                location.reload();
            } else {
                console.error('Error sending data:', xhr.statusText);
                alert('Error : ' + xhr.statusText);
            }
            document.getElementById('updateWifiCredBtnSpinner').style.display = 'none';
            document.getElementById('updateWifiCredBtn').disabled = false;
        };
        xhr.onerror = function() {
            console.error('Error sending data:', xhr.statusText);
            alert('Error : ' + xhr.statusText);
            document.getElementById('updateWifiCredBtnSpinner').style.display = 'none';
            document.getElementById('updateWifiCredBtn').disabled = false;
        };
        xhr.send(formData);
    });

    document.getElementById('updateMQTTDetailsBtn').addEventListener('click', function() {
        // Data to be sent
        document.getElementById('updateMQTTDetailsBtn').disabled = true;
        document.getElementById('updateMQTTDetailsBtnSpinner').style.display = 'block';
        
        // AJAX request
        var xhr = new XMLHttpRequest();
        xhr.open('POST', GLOBAL_host + '/api/updateMQTTDetails', true);
        //xhr.setRequestHeader('Content-Type', 'application/json');
        var formData = new FormData();
        formData.append('mqttHost', document.getElementById('mqttHost').value);
        formData.append('mqttPort', document.getElementById('mqttPort').value);
        formData.append('mqttUsername', document.getElementById('mqttUsername').value);
        formData.append('mqttPassword', document.getElementById('mqttPassword').value);
        xhr.onload = function() {
            if (xhr.status === 200) {
                console.log('Data sent successfully');
                alert('Updated Successfully - Status ' + JSON.parse(xhr.responseText).status);
                location.reload();
            } else {
                console.error('Error sending data:', xhr.statusText);
                alert('Error : ' + xhr.statusText);
            }
            document.getElementById('updateMQTTDetailsBtnSpinner').style.display = 'none';
            document.getElementById('updateMQTTDetailsBtn').disabled = false;
        };
        xhr.onerror = function() {
            console.error('Error sending data:', xhr.statusText);
            alert('Error : ' + xhr.statusText);
            document.getElementById('updateMQTTDetailsBtnSpinner').style.display = 'none';
            document.getElementById('updateMQTTDetailsBtn').disabled = false;
        };
        xhr.send(formData);
    });

    document.getElementById('restartDeviceBtn').addEventListener('click', function() {
        // AJAX request
        var xhr = new XMLHttpRequest();
        xhr.open('GET', GLOBAL_host + '/api/restartDevice', true);
        xhr.onload = function() {
            if (xhr.status === 200) {
                console.log('Successfully');
                alert('Restarted Successfully - Status ' + JSON.parse(xhr.responseText).status);
                location.reload();
            } else {
                console.error('Error :', xhr.statusText);
                alert('Error : ' + xhr.statusText);
            }
        };
        xhr.onerror = function() {
            console.error('Error :', xhr.statusText);
            alert('Error : ' + xhr.statusText);
        };
        xhr.send();
    });
});



function fetchDeviceData() {

    var xhr = new XMLHttpRequest();
    var url = GLOBAL_host + '/api/getDeviceDetails';
    xhr.onreadystatechange = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                // Request was successful, parse the response
                var data = JSON.parse(xhr.responseText);
                updateInnerHtmlForClassNames("_DeviceName", data.DeviceName);
                //updateInnerHtmlForClassNames("_DeviceName",data.DeviceName);
                updateInnerHtmlForClassNames("_DeviceId", data.DeviceID);
                updateInnerHtmlForClassNames("_DeviceFirmwareVersion", data.DeviceFirmwareVersion);
                updateInnerHtmlForClassNames("_DeviceFirmwareLastUpdatedOn", data.DeviceFirmwareLastUpdatedOn);
                updateInnerHtmlForClassNames("_DeviceHardware", data.DeviceHardware);
                updateInnerHtmlForClassNames("_DeviceManufacturer", data.DeviceManufacturer);
                // Process the JSON response data
                console.log(data);
            } else {
                // Request failed
                console.error('Request failed with status:', xhr);
                alert('Request failed with status:' + xhr.status);
            }
        }
    };
    xhr.open('GET', url, true);
    //xhr.setRequestHeader('Authorization', 'Basic ' + btoa('adminUsername:adminPassword'));
    xhr.send();
}
function fetchAvailableWifiData() {
    document.getElementById('refreshAvailableWifiListBtn').disabled = true;
    var xhr = new XMLHttpRequest();
    var url = GLOBAL_host + '/api/getAvailableWifiList';
    xhr.onreadystatechange = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                // Request was successful, parse the response
                var data = JSON.parse(xhr.responseText);
                const wifiList = document.getElementById('availableWifiList');
                wifiList.innerHTML = ''; // Clear existing rows

                data.networks.forEach(network => {
                    const row = document.createElement('tr');
                    const strengthClass = network.rssi;
                    row.innerHTML = `
                        <td><span>${getWifiSvgForSignal(network.rssi)}</span>${network.ssid}</td>
                        <td>${getEncryptionType(network.encryptionType)}</td>
                        <td><span class="${getStrengthLabel(network.rssi)}">${network.rssi} dBm</span></td>
                        <td><button onclick="connectToWifi('${network.ssid}')">Connect</button></td>
                    `;
                    wifiList.appendChild(row);
                });
                // Process the JSON response data
                document.getElementById('refreshAvailableWifiListBtn').disabled = false;
                console.log(data);
            } else {
                // Request failed
                console.error('Request failed with status:', xhr);
                alert('Request failed with status:' + xhr.status);
            }
        }
    };
    xhr.open('GET', url, true);
    //xhr.setRequestHeader('Authorization', 'Basic ' + btoa('adminUsername:adminPassword'));
    xhr.send();
}
function fetchWiFiData() {

    var xhr = new XMLHttpRequest();
    var url = GLOBAL_host + '/api/getWifiDetails';
    xhr.onreadystatechange = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                // Request was successful, parse the response
                var data = JSON.parse(xhr.responseText);
                updateValueForId("ssid", data.ssid);
                updateValueForId("password", data.password);
                updateInnerHtmlForClassNames("_WifiFirmware", data.firmwareVersion);
                updateInnerHtmlForClassNames("_WifiIP", data.ip);
                updateInnerHtmlForClassNames("_WifiMAC", data.mac);
                updateInnerHtmlForClassNames("_WifiStatus", data.status + " -> " + data.connectedTo);
                updateInnerHtmlForClassNames("_wifiCurrentStatus", data.status);
                updateInnerHtmlForClassNames("_WifiSignalStrength", data.signalStrength + " dBm");
                updateValueForId("apSSID", data.apSSID);
                updateValueForId("apPassword", data.apPassword);
                // Process the JSON response data
                console.log(data);
            } else {
                // Request failed
                console.error('Request failed with status:', xhr.status);
            }
        }
    };
    xhr.open('GET', url, true);
    //xhr.setRequestHeader('Authorization', 'Basic ' + btoa('adminUsername:adminPassword'));
    xhr.send();
}
function fetchMQTTData() {

        var xhr = new XMLHttpRequest();
        var url = GLOBAL_host + '/api/getMQTTDetails';
        xhr.onreadystatechange = function () {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                if (xhr.status === 200) {
                    // Request was successful, parse the response
                    var data = JSON.parse(xhr.responseText);
                    updateValueForId("mqttHost", data.mqttHost);
                    updateValueForId("mqttPort", data.mqttPort);
                    updateValueForId("mqttUsername", data.mqttUsername);
                    updateValueForId("mqttPassword", data.mqttPassword);
                    if(data.mqttStatus==true)
                    {
                        updateInnerHtmlForClassNames("_mqttStatus", "Connected");
                    }
                    else{
                        updateInnerHtmlForClassNames("_mqttStatus", "Not Connected");
                    }
                    // Process the JSON response data
                    console.log(data);
                } else {
                    // Request failed
                    console.error('Request failed with status:', xhr.status);
                }
            }
        };
        xhr.open('GET', url, true);
        //xhr.setRequestHeader('Authorization', 'Basic ' + btoa('adminUsername:adminPassword'));
        xhr.send();
    }
        
    function getSwitchStatusTagBasedOnValue(onValue, offValue, currentStatus, inverse) {
        if (inverse == true) {
            if (currentStatus == 1) {
                return `<span style='color:red; font-weight:bold;'>${offValue}</span>`;
            }
            else {
                return `<span style='color:green; font-weight:bold;'>${onValue}</span>`;
            }
        }
        else {
            if (currentStatus == 1) {
                return `<span style='color:green; font-weight:bold;'>${onValue}</span>`;
            }
            else {
                return `<span style='color:red; font-weight:bold;'>${offValue}</span>`;
            }
        }
    }
    function getToggleBasedOnState(onValue, offValue, currentStatus, inverse) {
        if (inverse == true) {
            if (currentStatus == 1) {
                return onValue;
            }
            else {
                return offValue;
            }
        }
        else {
            if (currentStatus == 1) {
                return offValue;
            }
            else {
                return onValue;
            }
        }
    }
    function toggleSwitch(id, stateValue, switchId) {
        // Data to be sent
        document.getElementById(switchId).disabled = true;

        // AJAX request
        var xhr = new XMLHttpRequest();
        xhr.open('POST', GLOBAL_host + '/api/toggleSwitch', true);
        //xhr.setRequestHeader('Content-Type', 'application/json');
        var formData = new FormData();
        formData.append('id', id);
        formData.append('stateValue', stateValue);
        xhr.onload = function () {
            if (xhr.status === 200) {
                console.log('Data sent successfully');
                //alert('Updated Successfully - Status ' + xhr.responseText);
                //location.reload();
                fetchSwitchesDetails();
            } else {
                console.error('Error sending data:', xhr.statusText);
                alert('Error : ' + xhr.statusText);
            }
            document.getElementById(switchId).disabled = false;
        };
        xhr.onerror = function () {
            console.error('Error sending data:', xhr.statusText);
            alert('Error : ' + xhr.statusText);
            document.getElementById(switchId).disabled = false;
        };
        xhr.send(formData);
    }
    function fetchSwitchesDetails() {
        var fileList = document.getElementById('switchesList');

        // AJAX request to fetch file list
        var xhr = new XMLHttpRequest();
        var url = GLOBAL_host + "/api/getSwitchStatusList";
        xhr.open('GET', url, true); // Adjust the URL as needed
        xhr.onload = function () {
            if (xhr.status === 200) {
                // Request was successful, parse the response
                var data = JSON.parse(xhr.responseText);
                const switchesList = document.getElementById('switchesList');
                switchesList.innerHTML = ''; // Clear existing rows

                data.switches.forEach(mySwitch => {
                    const row = document.createElement('tr');
                    const strengthClass = mySwitch.arrNo;
                    var toggleState = getToggleBasedOnState(mySwitch.payloadOn, mySwitch.payloadOff, mySwitch.status, mySwitch.inverseSwitch);
                    row.innerHTML = `
                    <td>${mySwitch.arrNo}</td>
                    <td>${mySwitch.name}</td>
                    <td>${mySwitch.pin}</td>
                    <td>${mySwitch.payloadOn}/${mySwitch.payloadOff}</td>
                    <td>${mySwitch.inverseSwitch}</td>
                    <td>${mySwitch.status}</td>
                    <td>${getSwitchStatusTagBasedOnValue(mySwitch.payloadOn, mySwitch.payloadOff, mySwitch.status, mySwitch.inverseSwitch)}</td>
                    <td><button class='btn btn-secondary' id="mySwitch_${mySwitch.arrNo}" onclick="toggleSwitch(${mySwitch.arrNo},'${toggleState}','mySwitch_${mySwitch.arrNo}')">${toggleState}</button></td>
                `;
                    switchesList.appendChild(row);
                });
            } else {
                console.error('Error:', xhr.statusText);
            }
        };
        xhr.onerror = function () {
            console.error('Error:', xhr.statusText);
        };
        xhr.send();
    }