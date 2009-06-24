/*
 * ieFixButtons jQuery plugin
 *
 * Copyright (c) 2007-2008 Michal Wojciechowski (odyniec.net)
 *
 * Dual licensed under the MIT (MIT-LICENSE.txt) 
 * and GPL (GPL-LICENSE.txt) licenses.
 *
 * http://odyniec.net/projects/iefixbuttons/
 *
 */

jQuery.fn.ieFixButtons = function () {
    return /MSIE [567]\./i.test(window.navigator.userAgent) ?
    this.find('button[type=submit]').click(function () {
        jQuery(this.form).bind('submit', { button: jQuery(this) }, function (event) {
            var $button = event.data.button;
            var m = $button.get(0).outerHTML.match(/value\s*=\s*['"]([^'"]*)['"]/);
            var val = (m ? m[1] : '');

            var $input = jQuery('<input type="hidden" ' +
                ($button.attr('name') ? 'name="' + $button.attr('name') + '" ' : '') +
                '" value="' + val + '" />');

            jQuery(this).append($input);

            jQuery.each(jQuery(this).find('button'), function () {
                jQuery(this).attr('name', '');
            });
        });
    })
    .end()
    : this;
};
