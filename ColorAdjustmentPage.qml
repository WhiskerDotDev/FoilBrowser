import QtQuick;
import QComponents;

Grid{
      id: root_grid;
      columns: 4;
      rows: 2;
      property point size;
      smooth: false; antialiasing: false;
      spacing: 4;
      property int childWidth:Math.floor(root_grid.height / root_grid.columns);
      property int childHeight: Math.floor( (root_grid.height / root_grid.rows) - root_grid.spacing );

      property real globalMin: -1.0;
      property real globalMax_RGB_ETC: 1.0;
      property real globalMax_SATURATION: 10;
      readonly property real m_ZERO: 0.0;

      property LabelledSpinBox brightness_control : LabelledSpinBox{
            label.text: "BRIGHTNESS";
            parent: root_grid;
            width: root_grid.childWidth; height: root_grid.childHeight;
            spinbox.min: root_grid.globalMin;
            spinbox.max: root_grid.globalMax_RGB_ETC;
      }
      property LabelledSpinBox saturation_control : LabelledSpinBox{
            label.text: "SATURATION";
            parent: root_grid;
            width: root_grid.childWidth; height: root_grid.childHeight;
            spinbox.min: root_grid.globalMin;
            spinbox.max: root_grid.globalMax_SATURATION;
      }
      property LabelledSpinBox contrast_control : LabelledSpinBox{
            label.text: "CONTRAST";
            parent: root_grid;
            width: root_grid.childWidth; height: root_grid.childHeight;
            spinbox.min: root_grid.globalMin;
            spinbox.max: root_grid.globalMax_RGB_ETC;
      }
      property LabelledSpinBox colorization_control : LabelledSpinBox{
            label.text: "TINT \nSTRENGTH";
            parent: root_grid;
            width: root_grid.childWidth; height: root_grid.childHeight;
            spinbox.min: root_grid.m_ZERO;
            spinbox.max: root_grid.globalMax_RGB_ETC;
      }
      property LabelledSpinBox colorization_R_control : LabelledSpinBox{
            label.text: "R_TINT";
            parent: root_grid;
            width: root_grid.childWidth; height: root_grid.childHeight;
            spinbox.min: root_grid.m_ZERO;
            spinbox.max: root_grid.globalMax_RGB_ETC;
      }
      property LabelledSpinBox colorization_G_control : LabelledSpinBox{
            label.text:"G_TINT";
            parent: root_grid;
            width: root_grid.childWidth; height: root_grid.childHeight;
            spinbox.min: root_grid.m_ZERO;
            spinbox.max: root_grid.globalMax_RGB_ETC;
      }
      property LabelledSpinBox colorization_B_control : LabelledSpinBox{
            label.text:"B_TINT";
            parent: root_grid;
            width: root_grid.childWidth; height: root_grid.childHeight;
            spinbox.min: root_grid.m_ZERO;
            spinbox.max: root_grid.globalMax_RGB_ETC;
      }
            Component.onCompleted: console.log("completed!");

}