<?php

function islandora_relationship_editor_add_relationships(array $form, array &$form_state, $object_pid) {
  $form = array();
  $form['this'] = array(
    '#title' => t('This Object'),
    '#type' => 'textfield',
    '#value' => $object_pid,
    '#disabled' => TRUE,
  );
  $form['relationship'] = array(
    '#type' => 'select',
    '#title' => t('Relationship'),
    '#options' => get_possible_relationships($object_pid),
  );

  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Weight'),
    '#description' => t('Objects can be ordered by weight, with lower numbers first. Must be integer, e.g. -1, 0, 1...'),
  );

  $options = array(
    'general' => t('Person'),
    'tune' => t('Tune'),
    'group' => t('Group'),
    'story' => t('Story Page'),
  );
  $form['target_type'] = array(
    '#type' => 'radios',
    '#options' => $options,
    '#default_value' => 'general',
    '#ajax' => array(
      'callback' => 'islandora_relationships_switch_callback',
      'wrapper' => 'target-field',
      'effect' => 'fade',
      'method' => 'replace',
    ),
  );
  $autocomplete_path = 'islandora/ethnography/autocomplete/' . (isset($form_state['values']['target_type']) ? $form_state['values']['target_type'] : 'general');
  $form['target'] = array(
    '#type' => 'textfield',
    '#title' => t('Target object'),
    '#autocomplete_path' => $autocomplete_path,
    '#prefix' => '<div id="target-field">',
    '#suffix' => '</div>',
  );
  $form['symmetric'] = array(
	  '#type' => 'fieldset',
	  '#title' => 'Symmetry',
    '#access' => FALSE,

  );
 $form['symmetric']['is-symmetric'] = array(
	  '#type' => 'checkbox',
	  '#title' => t('Add the corresponding relationship?'),
  );
  $form['symmetric']['sym-relationship'] = array(
	  '#type' => 'select',
	  '#title' => t('Choose the corresponding relationship'),
	  '#description' => t('This should be the "opposite" relationship, which will be applied to the target object, with a value of this object.'),
	  '#options' => get_possible_reverse_relationships($object_pid),
	  '#default_value' => '',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('submit'),
  );

  return $form;

}

function islandora_relationship_editor_add_relationships_submit($form, &$form_state) {
  $object = islandora_object_load($form_state['values']['this']);
  if (!$object) {
    drupal_set_message("Fedora object doesn't exist.");
  }

  // RM43776 Add weight as literal relation.
  // @todo: Add validation, if relation is hasWeight, value must be integer, non-empty...?
  // @todo: shift declaration of QSR_RELS_EXT_URI into more common module.
  if ($form_state['values']['relationship'] == 'qsr_rel:hasWeight') {
    $object->relationships->registerNamespace('qsr_rel', QSR_RELS_EXT_URI);
    $object->relationships->add(QSR_RELS_EXT_URI, 'hasWeight', $form_state['values']['weight'], RELS_TYPE_INT);
  }
  else {
    $target = islandora_object_load($form_state['values']['target']);
    if (!$target) {
      drupal_set_message("Fedora object doesn't exist.");
    }
    $object->relationships->add(MYNS_URI, $form_state['values']['relationship'], $form_state['values']['target']);

    if ($form_state['values']['is-symmetric'] == 1) {
      $target->relationships->add(MYNS_URI, $form_state['values']['sym-relationship'], $form_state['values']['this']);
    }
  }
  $form_state['redirect'] = "islandora/object/{$object->id}/manage/relationships";
}
