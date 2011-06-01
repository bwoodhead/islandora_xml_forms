<?php

// $Id$

/**
 * @file
 *
 */

/**
 * 
 */
class FormProcessAction {

  /**
   *
   * @var NodeRegistry
   */
  public $registry;
  /**
   * FormElement to be processed.
   * 
   * @var FormElement 
   */
  public $element;
  /**
   * Submitted Form Value.
   * 
   * @var mixed 
   */
  public $value;
  /**
   * Action to perform.
   * 
   * @var Action
   */
  protected $action;
  /**
   * Type of action defined by Action::CONST.
   * 
   * @var mixed 
   */
  public $type;

  /**
   *
   * @param NodeRegistry $registry
   * @param FormElement $element
   * @param mixed $value
   */
  public function __construct(NodeRegistry $registry, FormElement $element, $value = NULL) {
    $this->registry = $registry;
    $this->element = $element;
    $this->value = $value;
    list($this->type, $this->action) = $this->getTypeAndAction($registry, $element, $value);
  }

  /**
   *
   * @param NodeRegistry $registry
   * @param FormElement $element
   * @param mixed $value
   * @return array
   */
  private function getTypeAndAction(NodeRegistry $registry, FormElement $element, $value = NULL) {
    if (isset($element->actions)) {
      return $element->actions->getAppropriateAction($registry, $element, $value);
    }
    return array(Action::Undefined, NULL);
  }

  /**
   * 
   */
  public function execute() {
    if ($this->action) {
      $this->action->execute($this->registry, $this->element, $this->value);
    }
  }

}

/**
 * Process the form generate a Document.
 */
class FormProcessor {

  /**
   * Submitted form values.
   * 
   * @var FormValues 
   */
  protected $values;
  /**
   *
   * @var Document 
   */
  protected $registry;

  /**
   *
   * @param type $form_state
   * @param type $definition 
   */
  public function __construct(FormValues $values, NodeRegistry $registry) {
    $this->values = $values;
    $this->registry = $registry;
  }

  /**
   *
   * @param FormElement $element 
   */
  public function process(FormElements $elements) {
    $actions = &$this->createActions($elements);
    $actions = &$this->sortActions($actions);
    return $this->performActions($actions);
  }

  /**
   *
   * @param FormElements $elements
   * @return array
   */
  private function &createActions(FormElements $elements) {
    $actions = array();
    foreach ($elements as $element) {
      $actions[] = $this->createAction($element);
    }
    return $actions;
  }

  /**
   *
   * @param FormElement $element
   * @return FormProcessAction 
   */
  private function createAction(FormElement $element) {
    $value = $this->values->getValue($element);
    return new FormProcessAction($this->registry, $element, $value);
  }

  /**
   *
   * @param array $actions
   * @return array
   */
  private function &sortActions(array &$actions) {
    $delete = array();
    $update = array();
    $create = array();
    foreach ($actions as $action) {
      switch ($action->type) {
        case Action::Create:
          $create[] = $action;
          break;
        case Action::Update:
          $update[] = $action;
          break;
        case Action::Delete:
          $delete[] = $action;
          break;
      }
    }
    $actions = array_merge($update, $delete, $create);
    return $actions;
  }

  /**
   *
   * @param array $actions
   * @return Document
   */
  private function performActions(array &$actions) {
    foreach ($actions as $action) {
      $action->execute();
    }
    return $this->registry->document;
  }

}