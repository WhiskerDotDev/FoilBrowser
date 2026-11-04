// NOTE: This component and the accompanying .cpp file are planned to be moved into the QComponentsproj module.

//NOTE: I really wanna abandon this dumpster fire.
#pragma once

#include "intrin.h"
#include <qqml.h>
#include <QQuickItem>
#include <QObject>
#include <QPointer>
#include <QList>
#include <QProperty>
#include <QQmlProperty>
#include <QHash>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QVariantMap>
#include <QDebug>
// #include "qvariant.h"

class QQuickStack : public QQuickItem{
    Q_OBJECT
    // QML_ELEMENT
    QML_NAMED_ELEMENT(StackWidget)
    Q_PROPERTY(QQuickItem* currentTab READ getCurrentTab NOTIFY currentTabChanged FINAL)
    Q_PROPERTY(qint64 currentIndex READ getCurrentIndex WRITE switchToIndex NOTIFY currentIndexChanged FINAL)
public:

    Q_INVOKABLE void removeCacheItem(const QString& item){
        qint64 idx = defaultCacheItems.indexOf(item);
        if(idx != -1)defaultCacheItems.removeAt(idx);
    }

    Q_INVOKABLE void addCacheItem(const QString& item){ if(defaultCacheItems . contains(item) == false)defaultCacheItems.append(item); }

    // returns errorValue if read was unsuccessful
    Q_INVOKABLE QVariant readPropertyCache(const QString& propertyName, const QVariant& errorValue = false ){
        QQmlProperty prop = propertyCacheTable.value(propertyName);
        return prop.isValid()? prop.read() : errorValue;
    }

    // returns true if cache write was valid and successful, otherwise returns false.
    Q_INVOKABLE bool writePropertyCache(const QString& propertyName, const QVariant& data = QVariant(), const bool insertIfMissing = false){
        QQmlProperty prop = propertyCacheTable.value(propertyName);
        if( prop.isValid() == false ){
            if(insertIfMissing == true){
                QQmlProperty newProp(getCurrentTab(), propertyName);
                if(newProp.isValid() == true)propertyCacheTable.insert(propertyName, newProp);
                return true;
            }else
                if(insertIfMissing == false) return false;
        }
        prop.write(data);
        return true;
    }

    Q_INVOKABLE void rebuildPropertyCache(){
        this -> propertyCacheTable.clear();
        for( const QString& item : defaultCacheItems ){
            QQmlProperty prop(getCurrentTab(), item);
            if( prop.isValid() == true )propertyCacheTable.insert(item, prop);
        }
    }

    Q_INVOKABLE qint64 getCurrentIndex()const{return m_currentIndex;}
    Q_INVOKABLE void switchToIndex(qint64 idx){
        if(childItems().size() == 0) return; // if there are no children, why switch tabs at all?
        if(idx >= childItems().size())return;

        if(idx < 0) {idx = childItems().size() -1;} //wrap-around
        m_currentIndex = idx;
        this->ensureCorrectVisibility();

        emit this ->currentTabChanged(m_currentIndex);
        emit this -> currentIndexChanged(); // Note to self, do NOT connect a functor to BOTH the CurrentTabChanged and CurrentIndexChanged signals.
    }

    // Q_INVOKABLE QQuickItem* tabAtIndex(const qint64 &idx){return childItems().at(idx);}

    // Q_SIGNAL void requestTabRegistration(const qint64 &item_idx, qint64 destination_idx, bool focusOnCreation = true);
    // Q_INVOKABLE void registerTab (const qint64 &item_idx, qint64 destination_idx, bool focusOnCreation = true){
    //     // qDebug()<<"D_IDX: " << destination_idx<< " ARR SIZE: " << tabs.size();
    //     // qDebug()<<"ITM IDX: " << item_idx;
    //     // qDebug()<<"FAIL1";

    //     if( destination_idx > tabs.size() || destination_idx < 0 )destination_idx = tabs.size();
    //     childItems().constLast() -> setSize( this -> size() );
    //     QPointer<QQuickItem> ptr =  childItems().constLast();
    //     tabs.insert( destination_idx, ptr );
    //     qDebug() << "PTR: " << ptr << ptr.data()<< "SIZE:" << tabs.size();
    //     if(focusOnCreation == true)switchToIndex(childItems().size() -1); // this also sets m_currentIndex to ensure previous tabs are invisible and unfocused
    //     // if(tabs.size() > 1){
    //     // qDebug()<< "D_IDX: " << destination_idx;
    //     // qDebug()<< "INSERTED";
    //     // }else{
    //     // tabs.append(ptr);
    //     // qDebug() << "APPENDED";
    //     // }

    //     // qDebug()<<"FAIL2";

    //     // // qDebug()<<"FAIL4";
    //     // qDebug()<< "TAB:" << tabs.at(destination_idx);
    //     // qDebug() << "registering tab: " << childItems().at( item_idx ) << "to INDEX: " << destination_idx;
    // }

    Q_INVOKABLE void deleteTab( qint64 idx){
        if(idx <=0 || idx >= childItems().size() ){return;}
        childItems().at(idx)->deleteLater();
        // tabs.removeAt(idx);
        switchToIndex(m_currentIndex -1);
    }

    // Q_INVOKABLE qint64 childIndexOfTab(const qint64 &tab_Idx){return this -> childItems().indexOf(tabs.at(tab_Idx) .data());} // -1 if NF or error
    Q_INVOKABLE QQuickItem* getCurrentTab(){
        // qDebug()<<"current tab is: " << tabs.at( m_currentIndex );
        return childItems().isEmpty()? nullptr : childItems().at( m_currentIndex );
    }

    Q_INVOKABLE void ensureCorrectVisibility(){
        QQuickItem* currentChild = getCurrentTab();
        if(currentChild == nullptr) return;
        for(int i = 0; i < childItems().size(); i++){
            if(this -> childItems().at(i) != currentChild){ this ->childItems().at(i) -> setVisible(false); }
        }
        if(currentChild -> isVisible() == false)currentChild -> setVisible(true);
        if( childItems().at(m_currentIndex) ->size() != this ->size() ) childItems().at(m_currentIndex)->setSize(this->size());
        if(currentChild ->hasActiveFocus() == false) currentChild ->forceActiveFocus();
    }

    Q_INVOKABLE void registerComponent( const QByteArray& src, const QByteArray& nickname = ""){
        // qDebug()<<"CALLED";
        QPointer<QQmlComponent> ptr = new QQmlComponent(
            QQmlEngine::contextForObject(this)->engine()
            , src
            , QQmlComponent::CompilationMode::PreferSynchronous);

        componentTable.insertOrAssign({nickname == "" ? src : nickname}, ptr);
        qDebug()<<"COMPONENT: "<<ptr.data();
    }

    Q_INVOKABLE void createObject(const QByteArray& nickname, bool focusOnCreation, const QVariantMap& properties){
        // qDebug()<<"NOT READY!";
        if( componentTable[nickname].data()->isReady() == true){
            qDebug()<<"READY!";
            QQuickItem* obj =  qobject_cast<QQuickItem*>(componentTable[nickname]->createWithInitialProperties(properties));
            obj->setParentItem(this);
            switchToIndex(-1);
            qDebug()<<"CREATED: " <<obj << "THIS: " << this;
            // obj ->setParent(this);
            // qDebug()<<"CREATE_IDX : "<< preferredIndex;
            return;
        }
    }

    QQuickStack(QQuickItem* parent = nullptr){
        QObject::connect(this, &QQuickItem::heightChanged, [&](){
            QQuickItem* tab = getCurrentTab();
            if(tab != nullptr)tab->setHeight( this->height() );
        });
        QObject::connect(this, &QQuickItem::widthChanged, [&](){
            QQuickItem* tab = getCurrentTab();
            if(tab != nullptr)tab->setWidth( this->width() );
        });

        QObject::connect(this, &QQuickItem::visibleChildrenChanged, this,
                         [&](){  this -> ensureCorrectVisibility(); } );

        QObject::connect(this, &QQuickStack::currentTabChanged, this,[&](){ this -> rebuildPropertyCache(); });
        // QObject::connect(this, &QQuickItem::childrenChanged, this, [&](){
        //     qDebug()<<"children changed!";
        //     // this ->switchToIndex(-1);
        // });
    }

signals:
    void currentIndexChanged();
    void currentTabChanged(qint64 new_idx);

private:

    // const char* fancyName{ "registerTab"};
    // QList<QPointer<QQuickItem>> tabs;
    QList<QString> defaultCacheItems{"url", "loadProgress"};
    QHash< QByteArray , QPointer<QQmlComponent> > componentTable;
    QHash<QString, QQmlProperty> propertyCacheTable;
    qint64 m_currentIndex = 0;

    /*
STATES:

0 = free to use
1 = being written to
2 = being read from
*/
    // volatile char m_state = 0;
    // bool squeezeOnDeletion = false;
    // Keeping this around for convenience
    // To take .qml files as components and instantiate them in C++ correctly.
    // string may be the source of the file and/or a nickname for the type :)
    // will register the created object by default

    // QList<QPointer<QQmlComponent>> componentList;

    // ReadCacheEntry(error val?), WriteCacheEntry(override to add?)?
    // BASIC OPERATION:

    // This hashtable holds QQmlProperties for values for the current tab which are WIPED when the current tab is changed
    // I may re-build the property caches automatically via a QList (QStringList) property
    // I may also introduce a default or error value in case of invalid property read
    // THIS ONLY KEEPS VALID PROPERTIES!!!

    // List of properties to try and cache.
};
