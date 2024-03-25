using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections;
using System.Collections.Generic;

namespace BeanFactory
{
    public class Coroutine : YieldInstruction
    {
        public Coroutine(IEnumerator _enumerator) { enumerator = _enumerator; }

        IEnumerator enumerator;

        public override bool MoveNext()
        {
            //Check the stack
            if (enumerator.Current != null)
            {
                YieldInstruction instruction = enumerator.Current as YieldInstruction;
                //Parents managed to execute
                if (instruction.MoveNext() == true)
                    return true;
            }
            return enumerator.MoveNext();
        }

        public override void Reset()
        {
            enumerator.Reset();
        }
        //LinkedList
    }

    public class WaitForSeconds : YieldInstruction
    {
        public WaitForSeconds(float _seconds) { seconds = _seconds;}

        public override bool MoveNext()
        {
            timer += Time.deltaTime;
            return timer < seconds;
        }

        public override void Reset()
        {
            timer = 0f;
        }

        float timer = 0f;
        float seconds;
    }

    public class WaitForUnscaledSeconds : YieldInstruction
    {
        public WaitForUnscaledSeconds(float _seconds) { seconds = _seconds; }

        public override bool MoveNext()
        {
            timer += Time.unscaledDeltaTime;
            return timer < seconds;
        }

        public override void Reset()
        {
            timer = 0f;
        }

        float timer = 0f;
        float seconds;
    }

    public abstract class YieldInstruction : IEnumerator
    {
        public YieldInstruction() { }

        virtual public bool MoveNext(){ return false; }

        virtual public void Reset(){ }

        virtual public object Current
        {
            get{ return null; }
        }
    }
}
