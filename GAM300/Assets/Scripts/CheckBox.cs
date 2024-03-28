using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

class CheckBox : Script
{
    public AudioSource clickSound;

    public SpriteRenderer button;
    public GameObject selectedObj;

    private bool isChecked;

    void Start()
    {
        button = GetComponent<SpriteRenderer>();
    }

    void Update()
    {
        ButtonUpdate();
    }

    public void Check(bool check)
    {
        isChecked = check;
    }

    public bool IsChecked()
    {
        return isChecked;
    }

    void ButtonUpdate()
    {
        // Click sound
        if (button.IsButtonClicked())
        {
            if (clickSound != null)
                clickSound.Play();

            isChecked = !isChecked;
        }

        selectedObj.SetActive(isChecked);
    }
}
