/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "icecastmodel.h"
#include "icecastfilterwidget.h"
#include "ui_icecastfilterwidget.h"
#include "ui/iconloader.h"
#include "widgets/maclineedit.h"

#include <QMenu>
#include <QSignalMapper>

IcecastFilterWidget::IcecastFilterWidget(QWidget *parent)
  : QWidget(parent),
    ui_(new Ui_IcecastFilterWidget),
    sort_mode_mapper_(new QSignalMapper(this))
{
  ui_->setupUi(this);
  connect(ui_->clear, SIGNAL(clicked()), SLOT(ClearFilter()));

  // Icons
  ui_->clear->setIcon(IconLoader::Load("edit-clear-locationbar-ltr"));
  ui_->options->setIcon(IconLoader::Load("configure"));

  // Options actions
  QActionGroup* group = new QActionGroup(this);
  AddAction(group, ui_->action_sort_genre_popularity, IcecastModel::SortMode_GenreByPopularity);
  AddAction(group, ui_->action_sort_genre_alphabetically, IcecastModel::SortMode_GenreAlphabetical);
  AddAction(group, ui_->action_sort_station, IcecastModel::SortMode_StationAlphabetical);

  // Options menu
  QMenu* options_menu = new QMenu(this);
  options_menu->addActions(group->actions());
  ui_->options->setMenu(options_menu);

  connect(sort_mode_mapper_, SIGNAL(mapped(int)), SLOT(SortModeChanged(int)));

#ifdef Q_OS_DARWIN
  delete ui_->filter;
  MacLineEdit* lineedit = new MacLineEdit(this);
  ui_->horizontalLayout->insertWidget(1, lineedit);
  filter_ = lineedit;
  ui_->clear->setHidden(true);
#else
  filter_ = ui_->filter;
#endif
}

void IcecastFilterWidget::AddAction(
    QActionGroup* group, QAction* action, IcecastModel::SortMode mode) {
  group->addAction(action);
  sort_mode_mapper_->setMapping(action, mode);
  connect(action, SIGNAL(triggered()), sort_mode_mapper_, SLOT(map()));
}

IcecastFilterWidget::~IcecastFilterWidget() {
  delete ui_;
}

void IcecastFilterWidget::SetIcecastModel(IcecastModel* model) {
  model_ = model;
  connect(filter_->object(), SIGNAL(textChanged(QString)),
          model_, SLOT(SetFilterText(QString)));
}

void IcecastFilterWidget::ClearFilter() {
  filter_->clear();
  filter_->setFocus();
}

void IcecastFilterWidget::SortModeChanged(int mode) {
  model_->SetSortMode(IcecastModel::SortMode(mode));
}