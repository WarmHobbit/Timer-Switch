//List option geneeration
for (i = 0; i < 24; i++) {
	document.getElementById("duration").add(new Option(`${i}`, `${i}`));
}

const transmission = new Object(); //if this is const how am i able to modify it later?

document.getElementById("btn").addEventListener("click", (e) => {
	e.preventDefault();
	transmission.duration = document.getElementById("duration").value;
	transmission.unit = document.getElementById("unit").value;
	console.log(transmission);

	//sending data
	const xhr = new XMLHttpRequest();
	xhr.open("POST", "/control");
	xhr.setRequestHeader("Content-Type", "application/json");
	xhr.onreadystatechange = () => {
		switch (xhr.readyState) {
			case 0:
				console.log("State 0: Request Unsent");
				break;
			case 1:
				console.log("State 1: Request Opened");
				break;
			case 2:
				console.log("State 2: Headers Received");
				break;
			case 3:
				console.log("State 3: Loading Data");
				break;
			case 4:
				console.log("State 4: Complete!");
				console.log(xhr.responseText);
				break;
		}
	};
	xhr.send(JSON.stringify(transmission));

	//code to receive the data sent to the nodeMCU
	async function getStatus() {
		const response = await fetch("/status");
		const data = await response.json();
		document.getElementById("confirmation").textContent =
			`Timer set for ${data.rxdDuration} ${data.rxdUnit}.`;
	}

	setInterval(getStatus, 1000);

	// Emergency Stop
	stopBtn = document.getElementById("stop");
	stopBtn.addEventListener("click", () => {
		fetch("/reset", { method: "POST" });
	});
});
