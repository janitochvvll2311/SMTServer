const find = (id) => document.getElementById(id);

const fetchData = async () => {
    const url = `${window.location.hostname}/data`;
    const data = await (await fetch(url)).json();
    find('hostname').value = data.hostname;
    find('ssidname').value = data.ssidname;
    find('username').value = data.username;
}

window.onload = () => {
    fetchData();
}