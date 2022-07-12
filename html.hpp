#ifndef HTML_HPP
#define HTML_HPP

const auto HTML_LOGIN = "\n\
<!DOCTYPE html>\n\
<html lang='en'>\n\
\n\
<head>\n\
    <meta charset='UTF-8'>\n\
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>\n\
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n\
    <link rel='stylesheet' href='styles.css'>\n\
    <title>SMT RACK</title>\n\
</head>\n\
\n\
<body>\n\
    <form method='post' action='login' class='column-container'>\n\
        <label>\n\
            Username: <br />\n\
            <input name='username' type='text' />\n\
        </label>\n\
        <br />\n\
        <label>\n\
            Pssword: <br />\n\
            <input name='userpass' type='password' />\n\
        </label>\n\
        <br />\n\
        <input type='submit' value='login' />\n\
    </form>\n\
</body>\n\
\n\
</html>\n\
";

const auto CSS_STYLES = "\n\
* {\n\
    margin: 0;\n\
    padding: 0;\n\
    border-spacing: 0;\n\
}\n\
\n\
.column-container {\n\
    display: flex;\n\
    flex-direction: column;\n\
    align-items: center;\n\
}\n\
\n\
.row-container {\n\
    display: flex;\n\
    flex-direction: row;\n\
    align-items: center;\n\
}\n\
\n\
";

const auto HTML_CONFIG = "\n\
<!DOCTYPE html>\n\
<html lang='en'>\n\
\n\
<head>\n\
    <meta charset='UTF-8'>\n\
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>\n\
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n\
    <link rel='stylesheet' href='styles.css'>\n\
    <script src='scripts.js'></script>\n\
    <title>SMT RACK Config</title>\n\
</head>\n\
\n\
<body>\n\
    <form method='post' action='config' class='column-container'>\n\
        <div>\n\
            <h2>Wireless settings</h2>\n\
            <label>\n\
                Hostname: <br />\n\
                <input id='hostname' name='hostname' type='text' />\n\
            </label>\n\
            <br />\n\
            <label>\n\
                Host password: <br />\n\
                <input id='hostpass' name='hostpass' type='password' />\n\
            </label>\n\
            <br />\n\
            <label>\n\
                AP IP:\n\
                <input id='apip' name='apip' type='text' readonly/>\n\
            </label>\n\
            <br />\n\
            <label>\n\
                SSID name: <br />\n\
                <input id='ssidname' name='ssidname' type='text' />\n\
            </label>\n\
            <br />\n\
            <label>\n\
                SSID password: <br />\n\
                <input id='ssidpass' name='ssidpass' type='password' />\n\
            </label>\n\
            <br />\n\
            <label>\n\
                Local IP:\n\
                <input id='localip' name='localip' type='text' readonly/>\n\
            </label>\n\
            <br />\n\
        </div>\n\
        <div>\n\
            <h2>Admin settings</h2>\n\
            <label>\n\
                Username: <br />\n\
                <input id='username' name='username' type='text' />\n\
            </label>\n\
            <br />\n\
            <label>\n\
                User password: <br />\n\
                <input id='userpass' name='userpass' type='password' />\n\
            </label>\n\
        </div>\n\
        <br />\n\
        <input type='submit' value='save' />\n\
        <a href='/logout'>Logout</a>\n\
    </form>\n\
</body>\n\
\n\
</html>\n\
";

const auto JS_SCRIPTS = "\n\
const find = (id) => document.getElementById(id);\n\
\n\
const fetchData = async () => {\n\
    const url = `/data`;\n\
    const response = await fetch(url);\n\
    const data = await response.json();\n\
    find('hostname').value = data.hostname;\n\
    find('ssidname').value = data.ssidname;\n\
    find('username').value = data.username;\n\
    find('apip').value = data.apip;\n\
    find('localip').value = data.localip;\n\
}\n\
\n\
window.onload = () => {\n\
    fetchData();\n\
}\n\
";

#endif