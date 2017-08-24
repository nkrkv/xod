import R from 'ramda';
import React from 'react';
import PropTypes from 'prop-types';
import $ from 'sanctuary-def';
import { connect } from 'react-redux';
import { bindActionCreators } from 'redux';
import { HotKeys } from 'react-hotkeys';
import { Patch as PatchType } from 'xod-project';
import { $Maybe } from 'xod-func-tools';

import * as Actions from '../actions';
import * as ProjectActions from '../../project/actions';
import * as ProjectSelectors from '../../project/selectors';
import * as EditorSelectors from '../selectors';

import { COMMAND } from '../../utils/constants';
import { EDITOR_MODE } from '../../editor/constants';

import Patch from './Patch';
import NoPatch from '../components/NoPatch';
import Suggester from '../components/Suggester';
import ProjectBrowser from '../../projectBrowser/containers/ProjectBrowser';
import Sidebar from '../../utils/components/Sidebar';
import Workarea from '../../utils/components/Workarea';

import { RenderableSelection } from '../../project/types';
import sanctuaryPropType from '../../utils/sanctuaryPropType';

import Tabs from '../containers/Tabs';
import Inspector from '../components/Inspector';

class Editor extends React.Component {
  constructor(props) {
    super(props);

    this.setModeDefault = this.setModeDefault.bind(this);
    this.getHotkeyHandlers = this.getHotkeyHandlers.bind(this);
    this.onAddNode = this.onAddNode.bind(this);
    this.showSuggester = this.showSuggester.bind(this);
    this.hideSuggester = this.hideSuggester.bind(this);

    this.patchSize = this.props.size;
  }

  // TODO: Add possibility to place node at the slot when double click was produced
  //       (listen to double click in Patch and store in local state)
  onAddNode(patchPath) {
    // TODO: rewrite this when implementing "zombie" nodes
    this.hideSuggester();
    this.props.actions.addNode(
      patchPath,
      { x: 50, y: 50 },
      this.props.currentPatchPath
    );
  }

  setModeDefault() {
    this.props.actions.setMode(EDITOR_MODE.DEFAULT);
  }

  getHotkeyHandlers() {
    return {
      [COMMAND.SET_MODE_DEFAULT]: this.setModeDefault,
      [COMMAND.UNDO]: () => this.props.actions.undo(this.props.currentPatchPath),
      [COMMAND.REDO]: () => this.props.actions.redo(this.props.currentPatchPath),
      [COMMAND.INSERT_NODE]: () => this.showSuggester(),
    };
  }

  showSuggester() {
    this.props.actions.showSuggester();
  }

  hideSuggester() {
    this.props.actions.hideSuggester();
  }

  render() {
    const {
      currentPatchPath,
      currentPatch,
      selection,
      patchesIndex,
    } = this.props;

    const openedPatch = currentPatchPath
      ? (
        <Patch
          patchPath={currentPatchPath}
          size={this.patchSize}
        />
      ) : (
        <NoPatch />
      );

    const suggester = (this.props.suggesterIsVisible) ? (
      <Suggester
        index={patchesIndex}
        onAddNode={this.onAddNode}
        onBlur={this.hideSuggester}
      />
    ) : null;

    return (
      <HotKeys handlers={this.getHotkeyHandlers()} className="Editor">
        <Sidebar>
          <ProjectBrowser />
          <Inspector
            selection={selection}
            currentPatch={currentPatch}
            onPropUpdate={this.props.actions.updateNodeProperty}
            onPatchDescriptionUpdate={this.props.actions.updatePatchDescription}
          />
        </Sidebar>
        <Workarea>
          <Tabs />
          {openedPatch}
          {suggester}
        </Workarea>
      </HotKeys>
    );
  }
}

Editor.propTypes = {
  size: PropTypes.object.isRequired,
  selection: sanctuaryPropType($.Array(RenderableSelection)),
  currentPatchPath: PropTypes.string,
  currentPatch: sanctuaryPropType($Maybe(PatchType)),
  patchesIndex: PropTypes.object,
  suggesterIsVisible: PropTypes.bool,
  actions: PropTypes.shape({
    updateNodeProperty: PropTypes.func.isRequired,
    updatePatchDescription: PropTypes.func.isRequired,
    undo: PropTypes.func.isRequired,
    redo: PropTypes.func.isRequired,
    setMode: PropTypes.func.isRequired,
    addNode: PropTypes.func.isRequired,
    showSuggester: PropTypes.func.isRequired,
    hideSuggester: PropTypes.func.isRequired,
  }),
};

const mapStateToProps = R.applySpec({
  selection: ProjectSelectors.getRenderableSelection,
  currentPatch: ProjectSelectors.getCurrentPatch,
  currentPatchPath: EditorSelectors.getCurrentPatchPath,
  patchesIndex: ProjectSelectors.getPatchSearchIndex,
  suggesterIsVisible: EditorSelectors.getSuggesterVisibility,
});

const mapDispatchToProps = dispatch => ({
  actions: bindActionCreators({
    updateNodeProperty: ProjectActions.updateNodeProperty,
    updatePatchDescription: ProjectActions.updatePatchDescription,
    undo: ProjectActions.undoPatch,
    redo: ProjectActions.redoPatch,
    setMode: Actions.setMode,
    addNode: ProjectActions.addNode,
    showSuggester: Actions.showSuggester,
    hideSuggester: Actions.hideSuggester,
  }, dispatch),
});

export default connect(mapStateToProps, mapDispatchToProps)(Editor);
