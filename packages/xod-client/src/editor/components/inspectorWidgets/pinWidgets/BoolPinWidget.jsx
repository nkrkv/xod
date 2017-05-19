import React from 'react';

import { PROPERTY_TYPE } from '../../../constants';
import PinIcon from './PinIcon';
import LinkedInput from './LinkedInput';

function BoolWidget(props) {
  const onChange = (event) => {
    const boolValue = JSON.parse(event.target.value);
    props.onChange(boolValue);
  };

  const input = props.disabled
    ? (
      <LinkedInput id={props.elementId} />
    ) : (
      <select
        className="inspectorSelectInput"
        id={props.elementId}
        value={JSON.stringify(props.value)}
        onChange={onChange}
        onFocus={props.onFocus}
        onBlur={props.onBlur}
      >
        <option value="false">false</option>
        <option value="true">true</option>
      </select>
    );

  return (
    <div className="Widget PinWidget BoolWidget">
      {input}
      <PinIcon
        id={props.elementId}
        type={PROPERTY_TYPE.BOOL}
        isConnected={props.disabled}
      />
      <label
        htmlFor={props.elementId}
      >
        {props.label}
      </label>
    </div>
  );
}

BoolWidget.propTypes = {
  elementId: React.PropTypes.string.isRequired,
  label: React.PropTypes.string,
  value: React.PropTypes.bool,
  disabled: React.PropTypes.bool,
  onChange: React.PropTypes.func.isRequired,
  onFocus: React.PropTypes.func.isRequired,
  onBlur: React.PropTypes.func.isRequired,
};

BoolWidget.defaultProps = {
  label: 'Unnamed property',
  value: false,
  disabled: false,
};

export default BoolWidget;