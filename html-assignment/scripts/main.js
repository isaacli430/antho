"use strict";

/*pt1 js
* Select the 'hidden-image' element
* when click on get the element by section hides it
*/
let hiddenImage = document.getElementById('hidden-image');
hiddenImage.addEventListener('click', function() {
  this.classList.add('hide');
});

/*pt2 js 
* Select the 'changing-colours' section
* when click on it changes the background colour to deep pink
* when click on it again changes the background colour back to white
* when double click on it changes the text colour to purple
*/
let changingColoursSection = document.getElementById('changing-colours');

changingColoursSection.addEventListener('click', function() {
  this.classList.toggle('deep-pink');
});
changingColoursSection.addEventListener('dblclick', function() {
  this.style.color = 'purple';
});


/*pt3 js
* Select the input and paragraph elements
* when typing in the input element, the text is cloned in the paragraph element
* when pressing backspace, the last character is removed from the paragraph element
* when clicking the clear button, the input and paragraph elements are cleared
* when clicking the bold button, the first part of the text is bold
* when clicking the bold button, the button is hidden
* after clicking clear, the bold button is displayed again
*/
let inputElement = document.querySelector('#text-cloning input');
let paragraphElement = document.querySelector('#text-cloning p');


inputElement.addEventListener('keydown', function(event) {
  if (event.key.length === 1) {
    paragraphElement.textContent += event.key;
  } else if (event.key === 'Backspace') {
    paragraphElement.textContent = paragraphElement.textContent.slice(0, -1);
  }
});

function clearMsg(){ 
  inputElement.value = '';
  paragraphElement.textContent = '';
  inputElement.focus();
  document.getElementsByTagName('button')[1].style.display="block";
};

function boldMsg() {
  let text = paragraphElement.textContent;
  let middle = Math.floor(text.length / 2);
  let firstHalf = text.slice(0, middle);
  let secondHalf = text.slice(middle);
  firstHalf = `<span class="bold">${firstHalf}</span>`;
  paragraphElement.innerHTML = firstHalf + secondHalf;
  document.getElementsByTagName('button')[1].style.display="none";
}
/*pt4 js*
* Select the first and last list items
* when hovering over the first list item, change the background colour to pink
* when hovering over the last list item, change the background colour to pink
* when moving the mouse out of the first list item, remove the background colour
* when moving the mouse out of the last list item, remove the background colour
* when clicking on a list item, add a new list item after it. 
* The new list item should contain the text 'New Item'.
* can add multiple new items by clicking on different list items
*/
document.addEventListener("DOMContentLoaded", function() {
  var listItems = document.querySelectorAll("#list-things ul li");
  var firstItem = listItems[0];
  var lastItem = listItems[listItems.length - 1];

  firstItem.addEventListener("mouseover", function() {
      this.style.backgroundColor = "pink";
  });

  firstItem.addEventListener("mouseout", function() {
      this.style.backgroundColor = "";
  });

  lastItem.addEventListener("mouseover", function() {
      this.style.backgroundColor = "pink";
  });

  lastItem.addEventListener("mouseout", function() {
      this.style.backgroundColor = "";
  });
});

const list = document.querySelector('#list-things');

function addListItem(e) {
  if (e.target.tagName.toLowerCase() === 'li') {
    const newItem = document.createElement('li');
    newItem.textContent = 'New Item';
    e.target.parentNode.insertBefore(newItem, e.target.nextSibling);
  }
}

list.addEventListener('click', addListItem);

/*pt5 js
* Select the heading element
* when clicking on it, fetch the content from 'pt5.html' and replace the content of the parent div with the fetched content
* when an error occurs, log the error to the console
* when clicking on it again, the content is replaced with the original content
* when clicking on it again, the content is replaced with the fetched content
*/
document.querySelector('#simple-fetch h3').addEventListener('click', function(event) {
  event.preventDefault();
  var parentDiv = this.parentNode;

  fetch('pt5.html')
      .then(response => response.text())
      .then(data => {
          var parser = new DOMParser();
          var doc = parser.parseFromString(data, 'text/html');
          var fetchedContent = doc.querySelector('#simple-fetch-f2').innerHTML;
          parentDiv.innerHTML = fetchedContent;
      })
      .catch(error => console.error('Error:', error));
});


/*pt6 js*/

async function getConversionRate(baseCurrency, targetCurrency) {
  const API_KEY = "fca_live_F37O5UPnu2CGaGzBR9UjTPoOGvlPr30jOKXoMe1P";
  const response = await fetch(`https://api.freecurrencyapi.com/v1/latest?apikey=${API_KEY}`);
  const data = await response.json();
  const conversionRate = data.data[targetCurrency];
  return conversionRate;
}
  //this section is working, commented out is errors.

function submitCur() {
  const currencyInput = document.querySelector('#working-api input[type="text"]');
  const baseCurrencySelect = document.querySelectorAll('#working-api select')[0];
  const targetCurrencySelect = document.querySelectorAll('#working-api select')[1];
  const resultP = document.querySelector('#working-api p');

  // Get the input value
  let currencyValue = currencyInput.value.trim();

  // Check if the input is numeric
  if (isNaN(currencyValue) || currencyValue === '') {
      alert('Please enter a numeric value for currency.');
      return;
  }

  // Get the selected currencies
  let baseCurrency = baseCurrencySelect.value;
  let targetCurrency = targetCurrencySelect.value;

  // Check if the currencies are selected
  if (baseCurrency === '' || targetCurrency === '') {
      alert('Please select both base and target currencies.');
      return;
  }

  getConversionRate(baseCurrency, targetCurrency).then((conversionRate) => {
    // Calculate the converted value
    const convertedValue = currencyValue * conversionRate;

    // Display the converted value
    resultP.innerHTML = `<section class='bold'>Resulting Value:</section> $${convertedValue.toFixed(2)}`;
  });
}