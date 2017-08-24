import React from 'react';
import PropTypes from 'prop-types';

import classNames from 'classnames';
import Autosuggest from 'react-autosuggest';
import CustomScroll from 'react-custom-scroll';
import Highlighter from 'react-highlight-words';

import { KEYCODE } from '../../utils/constants';

const getSuggestionValue = ({ item }) => item.path;

class Suggester extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      value: '',
      suggestions: [],
    };

    this.input = null;

    this.renderItem = this.renderItem.bind(this);
    this.onChange = this.onChange.bind(this);
    this.onSuggestionsFetchRequested = this.onSuggestionsFetchRequested.bind(this);
    this.onSuggestionsClearRequested = this.onSuggestionsClearRequested.bind(this);
    this.onSuggestionSelected = this.onSuggestionSelected.bind(this);
    this.storeInputReference = this.storeInputReference.bind(this);
  }

  componentDidMount() {
    if (this.input) {
      // A hack to avoid typing "i" into input when pressing Hotkey
      setTimeout(() => this.input.focus(), 1);
    }
  }

  onChange(e, { newValue, method }) {
    if (
      method === 'up' ||
      method === 'down' ||
      method === 'click'
    ) {
      return;
    }

    this.setState({ value: newValue });
  }

  onSelect(value) {
    this.props.onAddNode(value);
  }

  onSuggestionsFetchRequested({ value, reason }) {
    if (reason === 'suggestion-selected') return;

    this.setState({
      suggestions: this.getSuggestions(value),
    });
  }

  onSuggestionsClearRequested() {
    this.setState({
      suggestions: [],
    });
  }

  onSuggestionSelected(event, { suggestionValue, method }) {
    if (method === 'enter') {
      this.onSelect(suggestionValue);
    }
  }

  getSuggestions(value) {
    const { index } = this.props;
    const inputValue = value.trim().toLowerCase();

    if (inputValue.length === 0) { return []; }

    const result = index.search(inputValue);
    const filtered = result.filter(res => res.score < 0.2);
    return (filtered.length > 1) ? filtered : result;
  }

  storeInputReference(autosuggest) {
    if (autosuggest !== null) {
      this.input = autosuggest.input;
    }
  }

  // eslint-disable-next-line class-methods-use-this
  renderItem({ item }, { isHighlighted }) {
    const cls = classNames('Suggester-item', {
      'is-highlighted': isHighlighted,
    });

    return (
      <div
        className={cls}
        onDoubleClick={(e) => {
          const val = getSuggestionValue({ item });
          this.onChange(e, { newValue: val, method: 'doubleClick' });
          this.onSelect(val);
        }}
      >
        <Highlighter
          className="path"
          searchWords={[this.state.value]}
          textToHighlight={item.path}
        />
        <Highlighter
          className="description"
          searchWords={[this.state.value]}
          textToHighlight={item.description}
        />
      </div>
    );
  }

  // eslint-disable-next-line class-methods-use-this
  renderSuggestionsContainer({ containerProps, children }) {
    return (
      <CustomScroll>
        <div
          {...containerProps}
          className="Suggester-container"
        >
          {children}
        </div>
      </CustomScroll>
    );
  }

  render() {
    const { value, suggestions } = this.state;

    const inputProps = {
      placeholder: 'Search nodes',
      value,
      onChange: this.onChange,
      onKeyDown: (event) => {
        const code = event.keyCode || event.which;
        if (code === KEYCODE.ESCAPE && event.target.value === '') {
          this.props.onBlur();
        }
      },
      onBlur: () => this.props.onBlur(),
      type: 'search',
    };

    return (
      <div className="Suggester">
        <Autosuggest
          suggestions={suggestions}
          value={value}
          inputProps={inputProps}
          alwaysRenderSuggestions
          highlightFirstSuggestion
          getSuggestionValue={getSuggestionValue}
          onSuggestionsFetchRequested={this.onSuggestionsFetchRequested}
          onSuggestionsClearRequested={this.onSuggestionsClearRequested}
          onSuggestionSelected={this.onSuggestionSelected}
          onSuggestionHighlighted={this.onSuggestionHighlighted}
          renderSuggestion={this.renderItem}
          renderSuggestionsContainer={this.renderSuggestionsContainer}
          ref={this.storeInputReference}
        />
      </div>
    );
  }
}

Suggester.defaultProps = {
  onBlur: () => {},
};

Suggester.propTypes = {
  index: PropTypes.object,
  onAddNode: PropTypes.func.isRequired,
  onBlur: PropTypes.func,
};

export default Suggester;
