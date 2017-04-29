using System;

using Xamarin.Forms;

namespace Hello_xamarin
{
	public class Myhomepage : ContentPage
	{
		public Myhomepage()
		{
			Content = new StackLayout
			{
				Children = {
					new Label { Text = "Hello ContentPage" }
				}
			};
		}
	}
}

