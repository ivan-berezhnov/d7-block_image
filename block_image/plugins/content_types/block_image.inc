<?php

/**
 * @file
 * Panels 'Block Image' content type for displaying an uploaded image.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */

$plugin = array(
    'title'           => t('Block Image'),
    'single'          => true,
    'render callback' => 'block_image_content_type_render',
    'icon'            => 'icon_block_image.svg',
    'description'     => t('Insert an bloc image anywhere in the template.'),
    'edit form'       => 'block_image_content_type_edit_form',
    'category'        => t('Custom')
);

/**
 * Render the content.
 *
 * @param $subtype
 * @param $conf
 * @param $args
 * @param $context
 *
 * @return \stdClass
 * @throws \Exception
 */
function block_image_content_type_render($subtype, $conf, $args, $context)
{
    $block = new stdClass();

    if (!empty($conf)) {
        $pid = $conf['image'];
        $file = file_load($pid);
        $uri = $file->uri;
        $image_url = file_create_url($uri);

        $block->content = theme('image_style',array(
            'style_name' => $conf['image_style'],
            'path' => $uri,
            'alt' => $conf['alt'],
            'title' => $conf['title'],
        ));
    }
    return $block;
}

/**
 * Returns an edit form for the custom type.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function block_image_content_type_edit_form($form, &$form_state)
{
    $form['#attributes']['enctype'] = array("multipart/form-data");

    form_load_include($form_state, 'inc', 'block_image', 'plugins/content_types/block_image');

    $conf = $form_state['conf'];

    $form['image'] = array(
        '#title' => t('Upload Image:'),
        '#type' => 'managed_file',
        '#description' => t('Upload an Image.'),
        '#default_value' => (!empty( $conf['image'])) ? $conf['image'] : '',
        '#upload_location' => 'public://block_image/',
        '#upload_validators' => array(
            'file_validate_extensions' => array('gif png jpeg jpg svg')
        ),
        '#required' => true,
    );

    $form['alt'] = array(
        '#type'          => 'textfield',
        '#title' => t('Image alt'),
        '#default_value' => (!empty($conf['alt'])) ? $conf['alt'] : '',
    );

    $form['title'] = array(
        '#type'          => 'textfield',
        '#title' => t('Image title'),
        '#default_value' => (!empty($conf['title'])) ? $conf['title'] : '',
    );

    $image_styles = image_styles();
    $image_style_options = array( 'none' => t('None') );
    foreach ( $image_styles as $name => $style ) {
        $image_style_options[$name] = $style['label'];
    }
    $form['image_style'] = array(
        '#type'          => 'select',
        '#title'         => t('Image Style'),
        '#default_value' => (!empty($conf['image_style'])) ? $conf['image_style'] : 'none',
        '#options'       => $image_style_options,
    );

    return $form;
}

/**
 * Submit callback.
 *
 * @param $form
 * @param $form_state
 */
function block_image_content_type_edit_form_submit($form, &$form_state)
{
    if (isset($form_state['image'])) {
        $file = $form_state['image'];
        // We are done with the file, remove it from storage.
        unset($form_state['image']);
        // Make the storage of the file permanent
        $file->status = FILE_STATUS_PERMANENT;
        // Save file status.
        file_save( $file );
        $form_state['values']['image'] = $file;
    }
    foreach (element_children( $form ) as $key) {
        if (isset( $form_state['values'][ $key ])) {
            $form_state['conf'][ $key ] = $form_state['values'][ $key ];
        }
    }
}
