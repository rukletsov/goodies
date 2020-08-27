// ==UserScript==
// @name         Intercept BRYTER tokens
// @namespace    http://tampermonkey.net/
// @version      1.0
// @description  Intercepts authentication tokens from header of the XHRs
// @author       Alexander Rukletsov
// @match        https://*.bryter.io/
// @run-at       document-start
// ==/UserScript==

(function(){
    'use strict'

    // Set up div section in the top left corner
    let div = document.createElement('div')
    let divStyle = div.style
    div.id = 'jwtdiv'
    div.style.position = 'absolute'
    div.style.top = '1%'
    div.style.left = '7%'
    div.style.zIndex = 2147483647
    document.body.appendChild(div);

    // Set up a button
    let btn = document.createElement('button')
    btn.id = 'jwtbtn'
    btn.innerHTML = 'Extract token(s)'
    btn.onclick = ButtonClickAction
    div.appendChild(btn);

    // Intercept and store tokens
    var observedTokens = new Set()

    var open = window.XMLHttpRequest.prototype.open
    window.XMLHttpRequest.prototype.open = function() {
        console.log('XHR request open: ', arguments)
        return open.apply(this, [].slice.call(arguments))
    }

    var headers = window.XMLHttpRequest.prototype.setRequestHeader
    window.XMLHttpRequest.prototype.setRequestHeader = function() {
        if (arguments[0] == 'Authorization') {
            console.log('Authorization header found: ', arguments)
            // Strip "Bearer " prefix
            let token = arguments[1].substring(7)
            observedTokens.add(token)
        }
        return headers.apply(this, [].slice.call(arguments))
    }

    // Button handler
    function ButtonClickAction(e) {
        switch (observedTokens.size) {
            case 0:
                document.getElementById('jwtbtn').innerHTML = 'No tokens found'
                break
            case 1:
                document.getElementById('jwtbtn').innerHTML = 'Token copied to clipboard'
                copyToClipboard(observedTokens.values().next().value)
                break
            default:
                document.getElementById('jwtbtn').innerHTML = `Found ${observedTokens.size} tokens:`
                for (let item of observedTokens) {
                    // TODO(alexr): Add a copy-to-clipboard button for each entry.
                    var div = document.createElement('div')
                    div.innerHTML = `${item}`
                    div.style.backgroundColor = 'Gainsboro'
                    div.style.margin = '5px'
                    document.getElementById('jwtdiv').appendChild(div)
                }
        }
    }

    // (c) Angelos Chalaris
    function copyToClipboard (str) {
        const el = document.createElement('textarea');
        el.value = str;
        el.setAttribute('readonly', '');
        el.style.position = 'absolute';
        el.style.left = '-9999px';
        document.body.appendChild(el);
        el.select();
        document.execCommand('copy');
        document.body.removeChild(el);
    }
}())