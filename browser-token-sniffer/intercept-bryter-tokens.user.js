// ==UserScript==
// @name         Intercept BRYTER tokens
// @namespace    http://tampermonkey.net/
// @version      1.0
// @description  Intercepts authentication tokens from header of the XHRs
// @author       Alexander Rukletsov
// @match        https://*.bryter.io/*
// @match        https://*.bryter.dev/*
// @run-at       document-start
// ==/UserScript==

/**
 * This script intercepts an auth token in a two-step algorithm:
 *
 *   1) Listen for an XHR to a specific URL and extract the token from the
 *      response;
 *
 *   2) If the first step yields nothing, intercept 'Authorization' headers of
 *      all XHRs the page makes, strip 'Bearer ' prefix, collect observed
 *      tokens. This is not ideal because it is hard to map headers to request
 *      URLs and other request data, which can be an issue if the page makes
 *      requests with distinct tokens.
 *
 * The script is not specific to BRYTER and can be used on any page; however,
 * the auth URL marker and response processing might require adjustment.
 */


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
    const authRequestMarker = 'openid-connect/token'
    const tokenFieldName = 'access_token'
    var authRequest = null
    var observedTokens = new Set()

    var open = window.XMLHttpRequest.prototype.open
    window.XMLHttpRequest.prototype.open = function() {
        let url = arguments[1]
        if (url.endsWith(authRequestMarker)) {
            // This request fetches the token we want, listen for the response.
            this.onreadystatechange = () => {
                console.log('XHR for auth token detected: ', this)
                authRequest = this
            }
        }

        return open.apply(this, [].slice.call(arguments))
    }

    var headers = window.XMLHttpRequest.prototype.setRequestHeader
    window.XMLHttpRequest.prototype.setRequestHeader = function() {
        if (arguments[0] == 'Authorization') {
            console.log('Authorization header observed: ', arguments)
            // Strip "Bearer " prefix.
            let token = arguments[1].substring(7)
            observedTokens.add(token)
        }
        return headers.apply(this, [].slice.call(arguments))
    }

    // Button handler
    function ButtonClickAction(e) {
        let checkAuthRequest = () =>
            authRequest && [JSON.parse(authRequest.responseText)[tokenFieldName]]

        let checkObservedTokens = () =>
            observedTokens && Array.from(observedTokens)

        let tokens = checkAuthRequest()
        tokens = tokens || checkObservedTokens()

        switch ((tokens || []).length) {
            case 0:
                document.getElementById('jwtbtn').innerHTML = 'No tokens found'
                break
            case 1:
                document.getElementById('jwtbtn').innerHTML = 'Token copied to clipboard'
                copyToClipboard(tokens[0])
                break
            default:
                document.getElementById('jwtbtn').innerHTML = `Found ${tokens.length} tokens:`
                for (let item of tokens) {
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
