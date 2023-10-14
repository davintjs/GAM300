using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BeanFactory
{
    public class LayerMask
    {
        int bitset;
        LayerMask() { bitset = 0; }

        static LayerMask GetMask(string[] layerNames)
        {
            LayerMask mask = new LayerMask();
            //Internal call to ask engine for the corresponding
            foreach (string layerName in layerNames)
            {
                int layer = InternalCalls.GetLayer(layerName);
                if (layer != -1)
                {
                    mask.bitset |= layer;
                }
                else
                {
                    //Console error
                }
            }
            return mask;
        }

        static string LayerToName (int layer)
        {
            return InternalCalls.GetLayerName(layer);
            //Internal call
        }

        static int NameToLayer (string name)
        {
            int layer = InternalCalls.GetLayer(name);
            if (layer != -1)
            {
                //Error message
            }
            return layer;
        }
    }
}
