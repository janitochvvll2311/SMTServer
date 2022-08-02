#ifndef HTML_HPP
#define HTML_HPP

const auto HTML_LOGIN = R"(
<!DOCTYPE html>
<html lang='en'>

<head>
    <meta charset='UTF-8'>
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <link rel='stylesheet' href='styles.css'>
    <title>SMT RACK</title>
</head>

<body>
    <form method='post' action='login' class='column-container'>
        <label>
            Username: <br />
            <input name='username' type='text' />
        </label>
        <br />
        <label>
            Pssword: <br />
            <input name='userpass' type='password' />
        </label>
        <br />
        <input type='submit' value='login' />
    </form>
</body>

</html>
)";

const auto CSS_STYLES = R"(
* {
    margin: 0;
    padding: 0;
    border-spacing: 0;
}

.column-container {
    display: flex;
    flex-direction: column;
    align-items: center;
}

.row-container {
    display: flex;
    flex-direction: row;
    align-items: center;
}

)";

const auto HTML_CONFIG = R"(
<!DOCTYPE html>
<html lang='en'>

<head>
    <meta charset='UTF-8'>
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <link rel='stylesheet' href='styles.css'>
    <script src='scripts.js'></script>
    <title>SMT RACK Config</title>
</head>

<body>
    <form method='post' action='config' class='column-container'>
        <div>
            <h2>Wireless settings</h2>
            <label>
                Hostname: <br />
                <input id='hostname' name='hostname' type='text' />
            </label>
            <br />
            <label>
                Host password: <br />
                <input id='hostpass' name='hostpass' type='password' />
            </label>
            <br />
            <label>
                AP IP:
                <input id='apip' name='apip' type='text' readonly/>
            </label>
            <br />
            <label>
                SSID name: <br />
                <input id='ssidname' name='ssidname' type='text' />
            </label>
            <br />
            <label>
                SSID password: <br />
                <input id='ssidpass' name='ssidpass' type='password' />
            </label>
            <br />
            <label>
                Local IP:
                <input id='localip' name='localip' type='text'/>
            </label>
            <br />
            <label>
                Gateway:
                <input id='gateway' name='gateway' type='text'/>
            </label>
            <br />
            <label>
                Mask:
                <input id='subnet' name='subnet' type='text'/>
            </label>
            <br />
        </div>
        <div>
            <h2>Light settings</h2>
            <label>
                LEDs per row: <br />
                <input id='ledcount' name='ledcount' type='number' />
            </label>
            <br />
            <label>
                Row count: <br />
                <input id='rowcount' name='rowcount' type='number' />
            </label>
        </div>
        <div>
            <h2>Admin settings</h2>
            <label>
                Username: <br />
                <input id='username' name='username' type='text' />
            </label>
            <br />
            <label>
                User password: <br />
                <input id='userpass' name='userpass' type='password' />
            </label>
        </div>
        <br />
        <input type='submit' value='save' />
        <a href='/logout'>Logout</a>
    </form>
</body>

</html>
)";

const auto JS_SCRIPTS = R"(
const find = (id) => document.getElementById(id);

const fetchData = async () => {
    const url = `/data`;
    const response = await fetch(url);
    const data = await response.json();
    find('hostname').value = data.hostname;
    find('ssidname').value = data.ssidname;
    find('username').value = data.username;
    find('apip').value = data.apip;
    find('localip').value = data.localip;
    find('ledcount').value = data.ledcount;
    find('rowcount').value = data.rowcount;
    find('localip').value = data.localip;
    find('gateway').value = data.gateway;
    find('subnet').value = data.subnet;
}

window.onload = () => {
    fetchData();
}
)";

#endif