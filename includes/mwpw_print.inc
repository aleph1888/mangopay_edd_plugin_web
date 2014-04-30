<?php

/**
 *
 * Provides some auxiliar functions related to echoing forms.
 *
 * @package     mwpw
 * @copyleft    Copyleft (l) 2014, Enredaos.net
 * @license     http://opensource.org/licenses/gpl-2.0.php GNU Public License
 * @since       0
 */

namespace mwpw;

/**
 *
 * Returns a LABEL and an INPUT TYPE CHECKBOX inside a <tr> tag with two <td> one for each.
 * Label will be localized as __( $name ).
 * Checkbox name attribute will be $name.
 * Also displays a SPAN tag for description using localized __( '{$name}_description', 'mwpw' ).
 *
 * @param string $name Also used for Input class
 * @param bool $is_checked
 *
 * @return string
 */
function mwpw_print_checkbox( $name, $is_checked ) {

	$checked = ( $is_checked  ? " checked='checked' " : null );

	return mwpw_print_row (
			mwpw_print_tag ( 'label', $name ),
			"<input name='{$name}' type='checkbox' {$checked} class='{$name}' />".
			"<span class='description'>" . __( "{$name}_description", 'mwpw') . "</span>"
	);

}


/**
 *
 * Returns a pair of LABELS inside a <TR> tag with two <td> one for each:
 * <tr>
 *		<td>
 *			<label><b>__( "mwpw_{$key}" , 'mwpw' )</label></b></label>
 *		</td>
 *		<td>
 *			<label>$value</label>
 *			<span class='description'>__( "{$key}_description", "mwpw" )</span>
 *		</td>
 *	</tr>
 *
 * Also displays a SPAN tag for description using:
 *
 * @param string $key For first label; will be localized as __( "mwpw_{$key}" )
 * @param string $value For second label (will be named with $key)
 * @param bool $show_description A localized span will be added to second label
 *
 * @return string
 */
function mwpw_print_label( $key, $value, $show_description = null ) {

	return mwpw_print_row (
		"<b>" . mwpw_print_tag( 'label', $key ) . "</b>",
		mwpw_print_tag ('label', $value, $key) .
		( $show_description ?
			"&nbsp;<span class='description'><i>" . __( "{$key}_description", 'mwpw') . "</i></span>\n" :
			null
		) );

}


/**
 *
 * Displays a LABEL and an INPUT TYPE TEXT inside a <TR> tag. Also displays a SPAN tag for description using __( {$key}_description )
 *
 * @param string $key For ID and NAME attributes prefixed with 'mwpw'
 * @param string $value
 *
 * @return string
 */
function mwpw_print_inputbox( $key, $value ) {

	return mwpw_print_row (
		mwpw_print_tag ('label', $key ) ,
		"<input type='text' id='mwpw_{$key}' name='mwpw_{$key}' value='{$value}' class='regular-text'>&nbsp;" .
		"<span class='description'>" . __( "{$key}_description", 'mwpw') . "</span>\n"
	);

}

/**
*
* Prints a TR tag with two TD one for each param
*/
function mwpw_print_row( $label, $field ) {

	return "\t<tr>\n" .
			"\t\t<td>{$label}</td>\n" .
			"\t\t<td>{$field}</td>\n" .
		"\t</tr>\n";

}


function mwpw_print_anchor( $name ) {

	return "<a name='{$name}'></a>\n";

}


function mwpw_print_link( $caption, $url ) {

	return "<a href='{$url}'>" . __( $caption, 'mwpw') . "</a>\n";

}


function mwpw_print_tag( $tag, $caption, $key = null ) {

	if ( $key )
		$class = "class='{$key}'";
	return "<{$tag} {$class}>" . __( $caption, 'mwpw') . "</{$tag}>\n{$description_output}";

}


/**
 *
 * Displays a TABLE header or footer tag. User $open bool to switch between them.
 *
 * @param string $class A valid css class
 * @param bool $open
 *
 * @return string
 */
function mwpw_print_table( $open, $class = 'form-table' ) {

	return  ( $open ? "<table class='{$class}'>\n" : "</table>\n" );

}


/**
 *
 * Displays a DIV header or footer tag. User $open bool to switch between them.
 *
 * @param bool $open
 * @param string $class A valid css class
 * @param bool $display If true then style.inline else style.none
 *
 * @return string
 */
function mwpw_print_div( $open, $class = null, $display = true) {

	$display =  ( $display ? 'inline' : 'none');
	return  ( $open ? "<div class='{$class}' style='display:{$display}'>\n"  : "</div>\n" );

}


/**
 *
 * Displays a INPUT TYPE BUTTON inside a <table>
 *
 * @param array $button [0] string __() key and also css class (note that will be sufixed with ' button' string)
 *                      [1] string url
 *
 * @return string
 */
function mwpw_print_button( $button ) {

	if ( $button ) {
		$output =  mwpw_print_table ( true );
		$output .=  mwpw_print_row ( "", "<input type='button' url='{$button[1]}' value='" .
						 __($button[0], 'mwpw') . "' class='{$button[0]} button'>");
		$output .=  mwpw_print_table ( false );
		return $output;
	}

}


/**
 *
 * Prints a submit button
 *
 * @params string $button Will be localized
 */
function mwpw_print_submit( $button ) {

	if ( $button )
		return mwpw_print_row ( "", "<input type='submit' value='" . __($button, 'mwpw') . "' >");


}


/**
 *
 * Prints a echoable DIV for pair of arrays $yFields-$defaults that represents form fields in read only mode
 *
 * @param string $type Represents form identification, will set DIV-class attribute, add an anchor and print a H4 tag with this
 * @params array $defaults List of fields names, will be localized, see $value param in mwpw_print_label() function
 * @params array $yFields List of fields names, will be localized, see $key param in mwpw_print_label() function
 * @params bool display Set div visibility
 * @params string $button Use it to add a button [see mwpw_print_submit]
 * @params bool $show_description Will hide $defaults description span
 * @params string $extras Any other html you wish to include
 */
function mwpw_print_section_info( $type, $defaults, $yFields, $display = true, $button = null, $show_description = true, $extras = null ) {

	$output = mwpw_print_div ( true, $type, $display ) .
			mwpw_print_anchor ( $type ) .
			mwpw_print_tag ( "h4", $type ) .
			mwpw_print_table ( true );
				foreach ( $yFields as $field )
 					$output .= mwpw_print_label ( $field, ( $defaults ? $defaults -> $field : null ), $show_description );
				$output .= $extras .
				mwpw_print_submit ( $button ) .
			mwpw_print_table ( false ) .
	 	mwpw_print_div ( false);
	return $output;

}


/**
 *
 * Prints a echoable DIV for pair of arrays $yFields-$defaults that represents form fields, like label/input
 *
 * @param string $type Represents form identification, will set DIV-class attribute, add an anchor and print a H4 tag with this
 * @params array $defaults List of fields names, will be localized, see $value param in mwpw_print_label() function
 * @params array $yFields List of fields names, will be localized, see $key param in mwpw_print_label() function
 * @params bool display Set div visibility
 * @params string $button Use it to add a button [see mwpw_print_submit]
 */
function mwpw_print_section($type, $defaults, $yFields, $display = true, $button = null) {

        $output = mwpw_print_div ( true, $type, $display ) .
                        mwpw_print_anchor ( $type ) .
                        mwpw_print_tag ( "h4", $type ) .
                        mwpw_print_table ( true );
                                foreach ( $yFields as $field )
                                        $output .= mwpw_print_inputbox ( $field, ( $defaults ? $defaults -> $field : null ) );
        $output .=              mwpw_print_submit ( $button ) .
                        mwpw_print_table ( false ) .
                mwpw_print_div ( false );
        return $output;

}


?>
