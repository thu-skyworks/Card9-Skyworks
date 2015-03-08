module.exports = {
	event: 0, 
	command: 1, 
	request: 2, 
	response: 3, 
	events: {
		cardDidScan: 0,
		doorDidOpen: 1,
		doorDidClose: 2,
		doorReleaseDidTriggered: 3,
		alarmDidOn: 4,
		alarmDidOff: 5,
		doorWillOpen: 6,
		alarmWillOff: 7,
		map: [
			"cardDidScan", 
			"doorDidOpen", 
			"doorDidClose", 
			"doorReleaseDidTriggered", 
			"alarmDidOn", 
			"alarmDidOff", 
			"doorWillOpen", 
			"alarmWillOff", 
		],
	},
	commands: {
		doDoorOpen: 0,
		doAlarmOn: 1,
		doAlarmOff: 2,
		heartBeep: 3,
		map: [
			"doDoorOpen",
			"doAlarmOn",
			"doAlarmOff",
			"heartBeep"
		],
	},
	requests: {
		auth: 0,
		map: [
			"auth",
		],
	},
	responses: {
		negative: 0,
		positive: 1,
		natural: 2,
		map: [
			"negative",
			"positive",
			"natural",
		],
	},
	map: ['event', 'command', 'request', 'response'],
}