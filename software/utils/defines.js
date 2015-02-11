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
	},
	commands: {
		doDoorOpen: 0,
		doAlarmOn: 1,
		doAlarmOff: 2,
	},
	requests: {
		auth: 0,
	},
	responses: {
		negative: 0,
		positive: 1,
		natural: 2,
	},
}