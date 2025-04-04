// ==UserScript==
// @name         Copy Jira Description Text
// @namespace    http://tampermonkey.net/
// @version      0.5
// @description  Copies text from all nested components of a specific div.
// @author       alexr and Gemini
// @match        https://issues.redhat.com/browse/*
// @grant        GM_setClipboard
// ==/UserScript==

(function() {
    'use strict';

    const targetDivClass = 'user-content-block';
    const copyButtonText = 'Copy Jira descripion';
    const copyAndAugmentButtonText = 'Copy Jira descripion with prompt';
    const prompt = `You're a thoughtful product owner and a former software engineer.
One of your product managers creates a feature request, quoted below after >>>.
Please analyse the request based on the following criteria and produce (this and only this, be brief!):

* an overall 'Jira Guide Score' (in format 'n out of 10') reflecting how well the request is written, with 0 being absolutely not well and 10 being couldn't be better

* a brief, maximum 3 item summary and maximum 2 sentences each about what could be improved for the feature request to be rated higher (focus on most important things)

Criteria:

1. Contains a goal summary, i.e. an elevator pitch (value statement) that describes the Feature in a clear, concise way

2. Goals and expected user outcomes, i.e. The observable functionality that the user now has as a result of receiving this feature. Include the anticipated primary user type/persona and which existing features, if any, will be expanded.

3. Success Criteria or KPIs measured, i.e. A list of specific, measurable criteria that will be used to determine if the feature is successful. Include key performance indicators (KPIs) or other metrics

4. Use Cases (Optional), i.e., use case diagrams, main success scenarios, alternative flow scenarios together with user type/persona involved.

>>>
`;

    function copyNestedDivText(addPrompt) {
        const targetDiv = document.querySelector(`div.${targetDivClass}`);

        if (targetDiv) {
            let textToCopy = '';
            const allTextNodes = getTextNodes(targetDiv);
            allTextNodes.forEach(node => {
                textToCopy += node.textContent + '\n'; // Add newline for better formatting
            });

            // Remove repeating newlines
            textToCopy = textToCopy.replace(/\n+/g, '\n').trim();

            // Augment with prompt if required
            if (addPrompt) {
                textToCopy = prompt + textToCopy
            };

            GM_setClipboard(prompt + textToCopy);
        } else {
            alert('Target div not found.');
        }
    }

    // Recursive function to get all text nodes within an element
    function getTextNodes(element) {
        let textNodes = [];
        for (let node of element.childNodes) {
            if (node.nodeType === Node.TEXT_NODE) {
                textNodes.push(node);
            } else if (node.nodeType === Node.ELEMENT_NODE) {
                textNodes = textNodes.concat(getTextNodes(node));
            }
        }
        return textNodes;
    }

    // Add buttons to the top of the page
    function addButtons() {
        const copyAndAugmentButton = document.createElement('button');
        copyAndAugmentButton.textContent = copyAndAugmentButtonText;
        copyAndAugmentButton.addEventListener('click', copyNestedDivText(true));
        document.body.insertBefore(copyAndAugmentButton, document.body.firstChild);

        const copyButton = document.createElement('button');
        copyButton.textContent = copyButtonText;
        copyButton.addEventListener('click', copyNestedDivText(false));
        document.body.insertBefore(copyButton, document.body.firstChild);
    }

    // Run addButton only after the DOM is fully loaded.
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', addButtons);
    } else {
        addButtons(); // DOMContentLoaded event already fired.
    }

})();