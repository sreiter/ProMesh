//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d09

#ifndef SCENE_ITEM_MODEL_H
#define SCENE_ITEM_MODEL_H

#include <QAbstractItemModel>
#include <QIcon>
#include <vector>
#include "scene/scene_interface.h"

////////////////////////////////////////////////////////////////////////
//	predeclarations
struct SceneItemInfo;

////////////////////////////////////////////////////////////////////////
//	typedefs
typedef std::vector<SceneItemInfo*> SceneItemInfoVec;

////////////////////////////////////////////////////////////////////////
//	constants
enum SceneItemType
{
	SIT_UNKNOWN = 0,
	SIT_OBJECT = 1,
	SIT_SUBSET = 2
};

enum SceneItemDataRole
{
	SIDR_VISIBLE = Qt::UserRole + 1,	//bool value
	SIDR_COLOR_SOLID,
	SIDR_COLOR_WIRE
};

////////////////////////////////////////////////////////////////////////
//	SceneItemInfo
///	holds information about the hierarchical representation of the item_model.
struct SceneItemInfo
{
	unsigned int type;
	SceneItemInfo* parent;
	SceneItemInfoVec children;

	ISceneObject* obj;///<	the associated scene-object (valid for subsets too).
	int index;///<	used as subset-index if type == SIT_SUBSET
};


////////////////////////////////////////////////////////////////////////
//	SceneItemModel
///	mediator between the scene and its gui-representation in a tree-view.
class SceneItemModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		SceneItemModel();
		virtual ~SceneItemModel();

		void setScene(IScene* scene);

		virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
		virtual QVariant headerData ( int section,
									  Qt::Orientation orientation,
									  int role = Qt::DisplayRole ) const;
		virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
		virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
		virtual QVariant data ( const QModelIndex & index,
								int role = Qt::DisplayRole ) const;
		virtual bool setData ( const QModelIndex & index,
							   const QVariant & value,
							   int role = Qt::EditRole );
		virtual QModelIndex index ( int row, int column,
									const QModelIndex & parent = QModelIndex() ) const;
		virtual QModelIndex parent ( const QModelIndex & index ) const;

		ISceneObject* objectFromIndex(const QModelIndex& index) const;
		QModelIndex parentObjectIndexFromIndex(const QModelIndex& index) const;

		void refreshSubsets();

	protected slots:
		void newObject(ISceneObject* obj);
		void removeObject(ISceneObject* obj);

	protected:
		SceneItemInfo* itemInfoFromIndex(const QModelIndex& index) const;
		QModelIndex indexFromItemInfo(SceneItemInfo* itemInfo,
									  int column) const;
		void updateItemInfo(SceneItemInfo* itemInfo);
		void eraseItemInfo(int index);

	protected:
		IScene* m_scene;
		SceneItemInfoVec	m_itemInfos;

	//	Icons
		QIcon	m_iconVisible;
		QIcon	m_iconInvisible;
		QIcon	m_iconColor;
};


#endif // SCENE_ITEM_MODEL_H
