// ==UserScript==
// @name         Merlin random roster
// @description  Determines who's from the Merlin team is going to run the meeting
// @author       Alexander Rukletsov
// @namespace    http://tampermonkey.net/
// @version      1.0
// @match        https://meet.google.com/*
// @grant        none
// ==/UserScript==

(function() {
    'use strict';

    // Set up div section in the top left corner.
    let div = document.createElement('div')
    div.style.position = 'absolute'
    div.style.backgroundColor = 'Gainsboro'
    div.style.top = '3%'
    div.style.left = '3%'
    div.style.zIndex = 2147483647
    document.body.appendChild(div)

    // Set up a button.
    let btn = document.createElement('button')
    btn.id = 'rosterbtn'
    btn.innerHTML = 'Who?'
    btn.onclick = who
    div.appendChild(btn)

    // Button handler.
    function who(e) {
        const roster = ['Alex', 'Daniel', 'Ivan', 'Micha&eumll', 'Mladen', 'Stephan', 'Tomek', 'Yann']
        const selectedOne = roster[Math.floor(Math.random() * roster.length)];
        document.getElementById('rosterbtn').innerHTML = selectedOne;
    }
})();